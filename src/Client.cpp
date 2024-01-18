#include "Client.h"

Client::Client(string const& _id, string const& _ip, string const& _mac) {
    client_id = _id;
    client_ip = _ip;
    client_mac = _mac;
}

ostream &operator<<(ostream &os, const Client &client) {
    os << "client_id: " << client.client_id << " client_ip: " << client.client_ip << " client_mac: "
       << client.client_mac << endl;
    return os;
}


Client::~Client() {

    // TODO: Free any dynamically allocated memory if necessary.
}

string Client::getClientId() const {
    return client_id;
}

string Client::getClientIp() const {
    return client_ip;
}

string Client::getClientMac() const {
    return client_mac;
}

unordered_map <string, string> Client::getRoutingTable() const {
    return routing_table;
}

queue<stack<Packet*>> Client::get_incoming_queue() const {
    return incoming_queue;
}
queue<stack<Packet*>> Client::get_outgoing_queue() const {
    return outgoing_queue;
}
