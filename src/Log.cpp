
#include "Log.h"

Log::Log(const string &_timestamp, const string &_message, int _number_of_frames, int _number_of_hops, const string &_sender_id,
         const string &_receiver_id, bool _success, ActivityType _type) {
    timestamp = _timestamp;
    message_content = _message;
    number_of_frames = _number_of_frames;
    number_of_hops = _number_of_hops;
    sender_id = _sender_id;
    receiver_id = _receiver_id;
    success_status = _success;
    activity_type = _type;
}




Log::~Log() {
    // TODO: Free any dynamically allocated memory if necessary.
}

string Log::getTimestamp() const {
    return timestamp;
}

string Log::getMessageContent() const {
    return message_content;
}

int Log::getNumberOfFrames() const {
    return number_of_frames;
}

int Log::getNumberOfHops() const {
    return number_of_hops;
}

string Log::getSenderId() const {
    return sender_id;
}

string Log::getReceiverId() const {
    return receiver_id;
}

bool Log::getSuccessStatus() const {
    return success_status;
}

ActivityType Log::getActivityType() const {
    return activity_type;
}