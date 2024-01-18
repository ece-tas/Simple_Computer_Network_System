#include <iostream>
#include "Network.h"
#include "Client.h"

using namespace std;

int main(int argc, char *argv[]) {
    //   C://homework_folder//Assignment3//sampleIO//1_sample_from_instructions//clients.dat C://homework_folder//Assignment3//sampleIO//1_sample_from_instructions//routing.dat C://homework_folder//Assignment3//sampleIO//1_sample_from_instructions//commands.dat 20 0706 0607
    //   C://homework_folder//Assignment3//sampleIO//2_routing_data_corrupted//clients.dat C://homework_folder//Assignment3//sampleIO//2_routing_data_corrupted//routing.dat C://homework_folder//Assignment3//sampleIO//2_routing_data_corrupted//commands.dat 20 0706 0607
    //  C://homework_folder//Assignment3//sampleIO//3_multiple_messages_corrupt_routing//clients.dat C://homework_folder//Assignment3//sampleIO//3_multiple_messages_corrupt_routing//routing.dat C://homework_folder//Assignment3//sampleIO//3_multiple_messages_corrupt_routing//commands.dat 10 895 7541

    // Instantiate HUBBMNET
    Network* HUBBMNET = new Network();

    // Read from input files
    vector<Client> clients = HUBBMNET->read_clients(argv[1]);
    /*for(auto& x : clients) {
        cout<<x.getClientId()<<", "<<x.getClientIp()<<", "<<x.getClientMac()<<endl;
    }*/
    HUBBMNET->read_routing_tables(clients, argv[2]);
   /* for(auto& x : clients) {
        for(auto& map : x.getRoutingTable()) {
            cout<<map.first<<", "<<map.second<<endl;
        } cout<<endl;
    }*/
    vector<string> commands = HUBBMNET->read_commands(argv[3]);
/*    for (int i = 0; i < commands.size(); ++i) {
        cout<<commands[i]<<endl;
    }*/
    // Get additional parameters from the cmd arguments
    int message_limit = stoi(argv[4]);
    string sender_port = argv[5];
    string receiver_port = argv[6];

    // Run the commands
    HUBBMNET->process_commands(clients, commands, message_limit, sender_port, receiver_port);

    // Delete HUBBMNET
    delete HUBBMNET;

    return 0;
}


