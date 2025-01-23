#include "PollConfig.h"

void setPollRate(int newPollRate) {
    pollRate = newPollRate;
}

int getPollRate() {
    return pollRate;
}

int getPollDelay() {
    return POLL_DELAY;
}