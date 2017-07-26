#include "HostLink.h"
#include "DebugLink.h"
#include "MemFileReader.h"
#include <boot.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <signal.h>

// Constructor
HostLink::HostLink()
{
  // Determine number of boards
  int numBoards = TinselMeshXLen * TinselMeshYLen + 1;

  // Create a DebugLink (UART) for each board
  debugLinks = new DebugLink [numBoards];

  // Initialise line buffers
  for (int x = 0; x < TinselMeshXLen; x++)
    for (int y = 0; y < TinselMeshYLen; y++)
      for (int c = 0; c < TinselCoresPerBoard; c++)
        for (int t = 0; t < TinselThreadsPerCore; t++)
          lineBufferLen[x][y][c][t] = 0;

  // Open each UART
  #ifdef SIMULATE
  // Worker boards
  int count = 0;
  for (int y = 0; y < TinselMeshYLen; y++)
    for (int x = 0; x < TinselMeshXLen; x++) {
      int boardId = y<<TinselMeshXBits + x;
      debugLinks[count++].open(boardId);
    }
  // Host board
  debugLinks[count++].open(-1);
  #else
  for (int i = 0; i < numBoards; i++) debugLinks[i].open(i+1);
  #endif

  // Initialise debug links
  bridgeBoard = NULL;
  for (int x = 0; x < TinselMeshXLen; x++)
    for (int y = 0; y < TinselMeshYLen; y++)
      mesh[x][y] = NULL;

  // Send query requests
  for (int i = 0; i < numBoards; i++)
    debugLinks[i].putQuery();

  // Get responses
  for (int i = 0; i < numBoards; i++) {
    uint32_t boardId;
    bool isHostBoard = !debugLinks[i].getQuery(&boardId);
    if (isHostBoard) {
      if (bridgeBoard != NULL) {
        fprintf(stderr, "Too many bridge boards detected\n");
        exit(EXIT_FAILURE);
      }
      bridgeBoard = &debugLinks[i];
    }
    else {
      uint32_t x = boardId % (1 << TinselMeshXBits);
      uint32_t y = boardId >> TinselMeshXBits;
      if (x >= TinselMeshXLen) {
        fprintf(stderr, "Mesh X dimension out of range: %d\n", x);
        exit(EXIT_FAILURE);
      }
      if (y >= TinselMeshYLen) {
        fprintf(stderr, "Mesh Y dimension out of range: %d\n", y);
        exit(EXIT_FAILURE);
      }
      if (mesh[x][y] != NULL) {
        fprintf(stderr, "Non-unique board id: %d\n", boardId);
        exit(EXIT_FAILURE);
      }
      mesh[x][y] = &debugLinks[i];
    }
  }

  // Lock file to ensure exclusive access to PCIeStream
  lockFile = open(PCIESTREAM_LOCK, O_RDONLY);
  if (lockFile < 0) {
    fprintf(stderr, "Error opening lock file %s\n", PCIESTREAM_LOCK);
    exit(EXIT_FAILURE);
  }
  if (flock(lockFile, LOCK_EX | LOCK_NB) != 0) {
    fprintf(stderr, "Can't acquire exclusive lock on PCIeStream\n");
    exit(EXIT_FAILURE);
  }

  // Ignore SIGPIPE
  signal(SIGPIPE, SIG_IGN);

  // Open PCIeStream for reading
  fromPCIe = open(PCIESTREAM_OUT, O_RDONLY);
  if (fromPCIe < 0) {
    fprintf(stderr, "Error opening %s\n", PCIESTREAM_OUT);
    exit(EXIT_FAILURE);
  }

  // Open PCIeStream for writing
  toPCIe = open(PCIESTREAM_IN, O_WRONLY);
  if (toPCIe < 0) {
    fprintf(stderr, "Error opening %s\n", PCIESTREAM_IN);
    exit(EXIT_FAILURE);
  }

  // Open PCIeStream control for reading
  fromPCIeCtrl = open(PCIESTREAM_CTRL_OUT, O_WRONLY);
  if (fromPCIeCtrl < 0) {
    fprintf(stderr, "Error opening %s\n", PCIESTREAM_CTRL_OUT);
    exit(EXIT_FAILURE);
  }

  // Open PCIeStream control for writing
  toPCIeCtrl = open(PCIESTREAM_CTRL_IN, O_WRONLY);
  if (toPCIeCtrl < 0) {
    fprintf(stderr, "Error opening %s\n", PCIESTREAM_CTRL_IN);
    exit(EXIT_FAILURE);
  }
}

// Destructor
HostLink::~HostLink()
{
  bridgeBoard->close();
  for (int x = 0; x < TinselMeshXLen; x++)
    for (int y = 0; y < TinselMeshYLen; y++)
      mesh[x][y]->close();
  delete [] debugLinks;
  flock(lockFile, LOCK_UN);
  close(lockFile);
  close(fromPCIe);
  close(toPCIe);
  close(fromPCIeCtrl);
  close(toPCIeCtrl);
}

// Address construction
uint32_t HostLink::toAddr(uint32_t meshX, uint32_t meshY,
             uint32_t coreId, uint32_t threadId)
{
  uint32_t addr;
  addr = meshY;
  addr = (addr << TinselMeshXBits) | meshX;
  addr = (addr << TinselLogCoresPerBoard) | coreId;
  addr = (addr << TinselLogThreadsPerCore) | threadId;
  return addr;
}

// Inject a message via PCIe (blocking)
void HostLink::send(uint32_t dest, uint32_t numFlits, void* payload)
{
  // Ensure that MaxFlitsPerMsg is not violated
  assert(numFlits > 0 && numFlits <= TinselMaxFlitsPerMsg);

  // We assume that message flits are 128 bits
  // (Because PCIeStream currently has this assumption)
  assert(TinselLogBytesPerFlit == 4);

  // Message buffer
  uint32_t buffer[4*(TinselMaxFlitsPerMsg+1)];

  // Fill in the message header
  // (See DE5HostTop.bsv for details)
  buffer[0] = dest;
  buffer[1] = 0;
  buffer[2] = (numFlits-1) << 24;
  buffer[3] = 0;

  // Bytes in payload
  int payloadBytes = numFlits*16;

  // Fill in message payload
  memcpy(&buffer[4], payload, payloadBytes);

  // Total bytes to send, including header
  int totalBytes = 16+payloadBytes;

  // We assume that totalBytes is less than PIPE_BUF bytes,
  // and write() will not send fewer PIPE_BUF bytes.
  int ret = write(toPCIe, buffer, totalBytes);
  if (ret != totalBytes) {
    fprintf(stderr, "Error writing to PCIeStream: totalBytes = %d, "
                    "PIPE_BUF=%d, ret=%d.\n", totalBytes, PIPE_BUF, ret);
    exit(EXIT_FAILURE);
  }
}

// Can send a message without blocking?
bool HostLink::canSend()
{
  pollfd pfd;
  pfd.fd = toPCIe;
  pfd.events = POLLOUT;
  return poll(&pfd, 1, 0) > 0;
}

// Extract a flit via PCIe (blocking)
void HostLink::recv(void* flit)
{
  int numBytes = 1 << TinselLogBytesPerFlit;
  uint8_t* ptr = (uint8_t*) flit;
  while (numBytes > 0) {
    int n = read(fromPCIe, (char*) ptr, numBytes);
    if (n <= 0) {
      fprintf(stderr, "Error reading from PCIeStream\n");
      exit(EXIT_FAILURE);
    }
    ptr += n;
    numBytes -= n;
  }
}

// Can receive a flit without blocking?
bool HostLink::canRecv()
{
  pollfd pfd;
  pfd.fd = fromPCIe;
  pfd.events = POLLIN;
  return poll(&pfd, 1, 0) > 0;
}

// Load application code and data onto the mesh
void HostLink::boot(const char* codeFilename, const char* dataFilename)
{
  MemFileReader code(codeFilename);
  MemFileReader data(dataFilename);

  // Request to boot loader
  BootReq req;

  // Total number of cores
  const uint32_t numCores =
    (TinselMeshXLen*TinselMeshYLen) << TinselLogCoresPerBoard;

  // Step 1: load code into instruction memory
  // -----------------------------------------

  uint32_t addrReg = 0;
  uint32_t addr, word;
  while (code.getWord(&addr, &word)) {
    // Send instruction to each core
    for (int x = 0; x < TinselMeshXLen; x++) {
      for (int y = 0; y < TinselMeshYLen; y++) {
        for (int i = 0; i < (1 << TinselLogCoresPerBoard); i++) {
          uint32_t dest = toAddr(x, y, i, 0);
          if (addr != addrReg) {
            req.cmd = SetAddrCmd;
            req.numArgs = 1;
            req.args[0] = addr;
            send(dest, 1, &req);
          }
          req.cmd = WriteInstrCmd;
          req.numArgs = 1;
          req.args[0] = word;
          send(dest, 1, &req);
        }
      }
    }
    addrReg = addr + 4;
  }

  // Step 2: initialise data memory
  // ------------------------------

  // Compute number of cores per DRAM
  const uint32_t coresPerDRAM = 1 <<
    (TinselLogCoresPerDCache + TinselLogDCachesPerDRAM);

  // Write data to DRAMs
  addrReg = 0;
  while (data.getWord(&addr, &word)) {
    for (int x = 0; x < TinselMeshXLen; x++) {
      for (int y = 0; y < TinselMeshYLen; y++) {
        for (int i = 0; i < TinselDRAMsPerBoard; i++) {
          // Use one core to initialise each DRAM
          uint32_t dest = toAddr(x, y, coresPerDRAM * i, 0);
          if (addr != addrReg) {
            req.cmd = SetAddrCmd;
            req.numArgs = 1;
            req.args[0] = addr;
            send(dest, 1, &req);
          }
          req.cmd = StoreCmd;
          req.numArgs = 1;
          req.args[0] = word;
          send(dest, 1, &req);
        }
      }
    }
    addrReg = addr + 4;
  }

  // Step 3: start cores
  // -------------------

  // Send start command
  uint32_t started = 0;
  uint8_t flit[4 << TinselLogWordsPerFlit];
  for (int x = 0; x < TinselMeshXLen; x++) {
    for (int y = 0; y < TinselMeshYLen; y++) {
      for (int i = 0; i < (1 << TinselLogCoresPerBoard); i++) {
        while (!canSend()) {
          if (canRecv()) {
            recv(flit);
            started++;
          }
        }
        uint32_t dest = toAddr(x, y, i, 0);
        req.cmd = StartCmd;
        req.args[0] = (1<<TinselLogThreadsPerCore)-1;
        send(dest, 1, &req);
      }
    }
  }

  // Wait for all start responses
  while (started < numCores) {
    recv(flit);
    started++;
  }
}

// Trigger to start application execution
void HostLink::go()
{
  for (int x = 0; x < TinselMeshXLen; x++) {
    for (int y = 0; y < TinselMeshYLen; y++) {
      mesh[x][y]->setBroadcastDest(0);
      mesh[x][y]->put(0);
    }
  }
}

// Load application code and data onto a single thread
void HostLink::bootOne(const char* codeFilename, const char* dataFilename)
{
  MemFileReader code(codeFilename);
  MemFileReader data(dataFilename);

  // Request to boot loader
  BootReq req;

  // Step 1: load code into instruction memory
  // -----------------------------------------

  uint32_t addrReg = 0;
  uint32_t addr, word;
  uint32_t dest = toAddr(0, 0, 0, 0);
  while (code.getWord(&addr, &word)) {
    // Write instruction
    if (addr != addrReg) {
      req.cmd = SetAddrCmd;
      req.numArgs = 1;
      req.args[0] = addr;
      send(dest, 1, &req);
    }
    req.cmd = WriteInstrCmd;
    req.numArgs = 1;
    req.args[0] = word;
    send(dest, 1, &req);
    addrReg = addr + 4;
  }

  // Step 2: initialise data memory
  // ------------------------------

  // Write data to DRAM
  addrReg = 0;
  while (data.getWord(&addr, &word)) {
    // Write data
    if (addr != addrReg) {
      req.cmd = SetAddrCmd;
      req.numArgs = 1;
      req.args[0] = addr;
      send(dest, 1, &req);
    }
    req.cmd = StoreCmd;
    req.numArgs = 1;
    req.args[0] = word;
    send(dest, 1, &req);
    addrReg = addr + 4;
  }

  // Step 3: start thread
  // --------------------

  // Send start command
  req.cmd = StartCmd;
  req.args[0] = 0;
  send(dest, 1, &req);

  // Wait for start response
  uint8_t flit[4 << TinselLogWordsPerFlit];
  recv(flit);
}

// Trigger to start application execution on a single thread
void HostLink::goOne()
{
  mesh[0][0]->setDest(0, 0);
  mesh[0][0]->put(0);
}

// Redirect UART StdOut to given file
// Returns false when no data has been emitted
bool HostLink::pollStdOut(FILE* outFile)
{
  bool got = false;
  for (int x = 0; x < TinselMeshXLen; x++) {
    for (int y = 0; y < TinselMeshYLen; y++) {
      if (mesh[x][y]->canGet()) {
        // Receive byte
        uint8_t byte;
        uint32_t c, t;
        mesh[x][y]->get(&c, &t, &byte);
        got = true;

        // Update line buffer & display on newline or buffer-full
        int len = lineBufferLen[x][y][c][t];
        if (byte == '\n' || len == MaxLineLen-1) {
          lineBuffer[x][y][c][t][len] = '\0';
          fprintf(outFile, "%d:%d:%d:%d: %s\n", x, y, c, t,
            lineBuffer[x][y][c][t]);
          lineBufferLen[x][y][c][t] = len = 0;
        }
        if (byte != '\n') {
          lineBuffer[x][y][c][t][len] = byte;
          lineBufferLen[x][y][c][t]++;
        }
      }
    }
  }

  return got;
}

// Redirect UART StdOut to stdout
// Returns false when no data has been emitted
bool HostLink::pollStdOut()
{
  pollStdOut(stdout);
}

// Redirect UART StdOut to given file (blocking function, never terminates)
void HostLink::dumpStdOut(FILE* outFile)
{
  for (;;) {
    bool ok = pollStdOut(outFile);
    if (!ok) usleep(10000);
  }
}

// Display UART StdOut (blocking function, never terminates)
void HostLink::dumpStdOut()
{
  dumpStdOut(stdout);
}