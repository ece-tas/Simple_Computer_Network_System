#ifndef PACKET_H
#define PACKET_H

#include <string>
#include <iostream>

using namespace std;

class Packet {
public:
    Packet(int layerID);
    virtual ~Packet();

    int layer_ID;

    friend ostream &operator<<(ostream &os, const Packet &packet);
    virtual void print() {};

    int get_layer_ID () const;
};

#endif // PACKET_H
