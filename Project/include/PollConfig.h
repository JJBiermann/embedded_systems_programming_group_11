#ifndef POLLCONFIG_H
#define POLLCONFIG_H

#include "Config.h"

static int pollRate;

void setPollRate(int newPollRate);

int getPollRate();

int getPollDelay();

#endif