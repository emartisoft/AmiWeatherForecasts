#ifndef ANNOUNCE_H
#define ANNOUNCE_H

struct Say {
    char espeakPath[512];
    char text[512];
    char language[3];
    int voice;
    int voicespeed;
    int hour;
    int minute;
};

enum SayType
{
    AN_TIME,
    AN_TEXT,
    AN_TEST
};

void executeSay(struct Say sy, enum SayType whatSay);

#endif