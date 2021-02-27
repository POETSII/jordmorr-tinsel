// SPDX-License-Identifier: BSD-2-Clause
#include <stdint.h>
#include "model.h"
/***************************************************
 * Edit values between here ->
 * ************************************************/
 
// Netlist
// Structure {ElementType, Input Places, Output Places}

//NodeTypes:
// 0 -> Pipeline
// 1 -> Synchroniser
// 2 -> Token Generator
// 3 -> Token Sink
// 4 -> Output Port

const uint32_t netlist[NOOFELEMENTS][7u] {
    {1, 0, 3, 5, 1, 2, 4},                    //0
    {0, 2, 7, 3, 6},                          //1
    {0, 4, 9, 5, 8},                          //2
    {0, 6, 11, 7, 10},                        //3
    {0, 8, 13, 9, 12},                        //4
    {0, 10, 15, 11, 14},                      //5
    {0, 12, 17, 13, 16},                      //6
    {0, 14, 19, 15, 18},                      //7
    {0, 16, 21, 17, 20},                      //8
    {1, 18, 20, 23, 19, 21, 22},              //9
    {1, 22, 25, 27, 23, 24, 26},             //10
    {0, 24, 29, 25, 28},                     //11
    {0, 26, 31, 27, 30},                     //12
    {0, 28, 33, 29, 32},                     //13
    {0, 30, 35, 31, 34},                     //14
    {0, 32, 37, 33, 36},                     //15
    {0, 34, 39, 35, 38},                     //16
    {0, 36, 41, 37, 40},                     //17
    {0, 38, 43, 39, 42},                     //18
    {1, 40, 42, 45, 41, 43, 44},             //19
    {1, 44, 47, 49, 45, 46, 48},             //20
    {0, 46, 51, 47, 50},                     //21
    {0, 48, 53, 49, 52},                     //22
    {0, 50, 55, 51, 54},                     //23
    {0, 52, 57, 53, 56},                     //24
    {0, 54, 59, 55, 58},                     //25
    {0, 56, 61, 57, 60},                     //26
    {0, 58, 63, 59, 62},                     //27
    {0, 60, 65, 61, 64},                     //28
    {1, 62, 64, 67, 63, 65, 66},             //29
    {1, 66, 69, 71, 67, 68, 70},             //30
    {0, 68, 73, 69, 72},                     //31
    {0, 70, 75, 71, 74},                     //32
    {0, 72, 77, 73, 76},                     //33
    {0, 74, 79, 75, 78},                     //34
    {0, 76, 81, 77, 80},                     //35
    {0, 78, 83, 79, 82},                     //36
    {0, 80, 85, 81, 84},                     //37
    {0, 82, 87, 83, 86},                     //38
    {1, 84, 86, 89, 85, 87, 88},             //39
    {1, 88, 91, 93, 89, 90, 92},             //40
    {0, 90, 95, 91, 94},                     //41
    {0, 92, 97, 93, 96},                     //42
    {0, 94, 99, 95, 98},                     //43
    {0, 96, 101, 97, 100},                   //44
    {0, 98, 103, 99, 102},                   //45
    {0, 100, 105, 101, 104},                 //46
    {0, 102, 107, 103, 106},                 //47
    {0, 104, 109, 105, 108},                 //48
    {1, 106, 108, 111, 107, 109, 110},       //49
    {2, 0},                      //0a
    {2, 111},                    //1a
    {3, 1},                      //2a
    {3, 110},                    //3a
    {4, 1, 111, 0},              //4a
    {4, 100, 0, 1},              //5a
};

// Intial Place Markings

const uint32_t place[NOOFPLACES] {
    1, //0
    0, //1
    
    0, //2
    1, //3
    0, //4
    1, //5
    1, //6
    0, //7
    1, //8
    0, //9
    0, //10
    1, //11
    0, //12
    1, //13
    1, //14
    0, //15
    1, //16
    0, //17
    0, //18
    1, //19
    0, //20
    1, //21
    
    1, //22
    0, //23
    
    0, //24
    1, //25
    0, //26
    1, //27
    1, //28
    0, //29
    1, //30
    0, //31
    0, //32
    1, //33
    0, //34
    1, //35
    1, //36
    0, //37
    1, //38
    0, //39
    0, //40
    1, //41
    0, //42
    1, //43
    
    1, //44
    0, //45
    
    0, //46
    1, //47
    0, //48
    1, //49
    1, //50
    0, //51
    1, //52
    0, //53
    0, //54
    1, //55
    0, //56
    1, //57
    1, //58
    0, //59
    1, //60
    0, //61
    0, //62
    1, //63
    0, //64
    1, //65
    
    1, //66
    0, //67
    
    0, //68
    1, //69
    0, //70
    1, //71
    1, //72
    0, //73
    1, //74
    0, //75
    0, //76
    1, //77
    0, //78
    1, //79
    1, //80
    0, //81
    1, //82
    0, //83
    0, //84
    1, //85
    0, //86
    1, //87
    
    1, //88
    0, //89
    
    0, //90
    1, //91
    0, //92
    1, //93
    1, //94
    0, //95
    1, //96
    0, //97
    0, //98
    1, //99
    0, //100
    1, //101
    1, //102
    0, //103
    1, //104
    0, //105
    0, //106
    1, //107
    0, //108
    1, //109
    
    0, //110
    0, //111
};

// Cluster Layout

// Intraboard Mailbox Connections per Board 
const uint8_t mailboxPath[TinselMailboxesPerBoard][2u] = { 
    { 0, 0 },
    { 0, 1 },
    { 0, 2 },
    { 0, 3 },
    { 1, 3 },
    { 1, 2 },
    { 1, 1 },
    { 1, 0 },
    { 2, 0 },
    { 2, 1 },
    { 2, 2 },
    { 2, 3 },
    { 3, 3 },
    { 3, 2 },
    { 3, 1 },
    { 3, 0 }
};

// Intrabox Board Connections per Box 
const uint8_t boardPath[(TinselBoardsPerBox - 1u) * NOOFBOXES][2u] = { 
    { 0, 0 },
    { 1, 0 },
    { 2, 0 },
    { 3, 0 },
    { 4, 0 },
    { 5, 0 },
    { 5, 1 },
    { 4, 1 },
    { 3, 1 },
    { 3, 2 },
    { 4, 2 },
    { 5, 2 },
    { 5, 3 },
    { 4, 3 },
    { 3, 3 },
    { 3, 4 },
    { 4, 4 },
    { 5, 4 },
    { 5, 5 },
    { 4, 5 },
    { 3, 5 },
    { 3, 6 },
    { 4, 6 },
    { 5, 6 },
    { 5, 7 },
    { 4, 7 },
    { 3, 7 },
    { 2, 7 },
    { 1, 7 },
    { 0, 7 },
    { 0, 6 },
    { 1, 6 },
    { 2, 6 },
    { 2, 5 },
    { 1, 5 },
    { 0, 5 },
    { 0, 4 },
    { 1, 4 },
    { 2, 4 },
    { 2, 3 },
    { 1, 3 },
    { 0, 3 },
    { 0, 2 },
    { 1, 2 },
    { 2, 2 },
    { 2, 1 },
    { 1, 1 },
    { 0, 1 }
};

/***************************************************
 * <- And here
 * ************************************************/

