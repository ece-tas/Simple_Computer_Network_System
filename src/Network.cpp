#include "Network.h"
#include "Log.h"
#include <iostream>
#include <sstream>
#include <ostream>
#include <fstream>
#include <unordered_map>
#include <map>

using namespace std;

Network::Network() {

}

void Network::process_commands(vector<Client> &clients, vector<string> &commands, int message_limit,
                               const string &sender_port, const string &receiver_port) {
    // TODO: Execute the commands given as a vector of strings while utilizing the remaining arguments.
    /* Don't use any static variables, assume this method will be called over and over during testing.
     Don't forget to update the necessary member variables after processing each command. For example,
     after the MESSAGE command, the outgoing queue of the sender must have the expected frames ready to send. */


    map<string, string> status;
    string sender_id;
    string receiver_id;

    string message_content;
    int number_of_frames = 0;
    int this_frame_number;

    for (auto& command : commands) {
        cout<<string(command.size() + 9, '-')<<endl<<"Command: "<<command<<endl<<string(command.size() + 9, '-')<<endl;

        if (command.substr(0, 7) == "MESSAGE") {
            string next_hop_id;
            string frame_message;
            sender_id = command.substr(8, 1);
            receiver_id = command.substr(10, 1);
            int hopNumber;
            string senderMac, receiverMac, senderIp, receiverIp;

            size_t position = command.find("#");
            size_t last_position = command.substr(position + 1).find('#');
            char punctuation_mark = command[command.size() - 2];
            if (punctuation_mark == '!' || punctuation_mark == '.' || punctuation_mark == '?') {
                message_content = command.substr(position + 1, last_position);
                int length_msg = message_content.size();

                cout<<"Message to be sent: "<<message_content<<endl<<endl;
                for (auto& client : clients) {
                    for (auto &route: client.getRoutingTable()) {
                        if (client.getClientId() == sender_id) {
                            hopNumber = client.number_of_hops;
                            senderMac = client.getClientMac();
                            senderIp = client.getClientIp();

                            if (route.first == receiver_id && route.second == receiver_id) {
                                next_hop_id = route.second;
                            } else if (route.first == receiver_id && route.second != receiver_id) {
                                next_hop_id = route.second;
                            }
                            for (auto& mac : clients) {
                                if (next_hop_id == mac.getClientId()) {
                                    receiverMac = mac.getClientMac();
                                    break;
                                }
                            }
                        }
                        if (client.getClientId() == receiver_id) {

                            // receiverMac = client.getClientMac();
                            receiverIp = client.getClientIp();
                        }
                    }
                }


                number_of_frames += (length_msg % message_limit == 0) ? (length_msg / message_limit) : (length_msg / message_limit + 1);
                this_frame_number = (length_msg % message_limit == 0) ? (length_msg / message_limit) : (length_msg / message_limit + 1);
                for (int i = 0; i < this_frame_number; ++i) {

                    if (i == this_frame_number - 1) {
                        frame_message = message_content.substr(i*message_limit,length_msg);

                    } else {
                        frame_message = message_content.substr(i*message_limit,message_limit);

                    }
                    cout<<"Frame #"<<i + 1<<endl;
                    Packet* applicationLayerPacket = new ApplicationLayerPacket(0, sender_id, receiver_id, frame_message);
                    Packet* transportLayerPacket = new TransportLayerPacket(1, sender_port, receiver_port);
                    Packet* networkLayerPacket = new NetworkLayerPacket(2, senderIp, receiverIp);
                    Packet* physicalLayerPacket = new PhysicalLayerPacket(3, senderMac, receiverMac);

                    frame.push(applicationLayerPacket);
                    frame.push(transportLayerPacket);
                    frame.push(networkLayerPacket);
                    frame.push(physicalLayerPacket);

                    physicalLayerPacket->print();
                    networkLayerPacket->print();
                    transportLayerPacket->print();
                    applicationLayerPacket->print();
                    cout<<"Number of hops so far: "<<hopNumber<<endl;
                    cout<<string(to_string(i).size() + 7, '-')<<endl;

                    for (auto& client : clients) {
                        if (sender_id == client.getClientId()) {
                            client.outgoing_queue.push(frame);
                        }
                    }
                }


            } else {
                // cout<<"The message does not have an end."<<endl;
            }

        } else if (command.substr(0, 15) == "SHOW_FRAME_INFO") {
            string line = command;
            istringstream iss(line);
            vector<string> tokens;
            string carriedMessage, print0, senderId;
            int hopNumber;

            // Split the line into tokens based on spaces
            while (iss >> line) {
                tokens.push_back(line);
            }
            string clientName = tokens[1];
            string statusType = tokens[2];
            int frameNumber = stoi(tokens[3]);
            bool outExist = false;
            bool inExist = false;
            ApplicationLayerPacket* appLayerPacket;
            TransportLayerPacket* transLayerPacket;
            NetworkLayerPacket* netLayerPacket;
            PhysicalLayerPacket* physLayerPacket;

            for (auto& client : clients) {
                if (clientName == client.getClientId()) {
                    queue<stack<Packet*>> copy_queue;
                    if (!client.incoming_queue.empty()) {
                        inExist = true;
                    }
                    if (!client.outgoing_queue.empty()) {
                        outExist = true;
                    }
                    if (outExist && statusType == "out" || inExist && statusType == "in") {
                        if (statusType == "out")
                            copy_queue = client.get_outgoing_queue();
                        else if (statusType == "in")
                            copy_queue = client.get_incoming_queue();


                        // no error up to here
                        stack<Packet*> copy_stack;
                        for (int i = 1; i <= number_of_frames; ++i) {

                            if (!copy_queue.empty()) {
                                copy_stack = copy_queue.front();
                                copy_queue.pop();
                            }
                            if (frameNumber == i) {
                                // stack<Packet*> copy_stack = copy_queue.front();
                                // up to here, there is memory error and leak

                                for (int j = 3; j >= 0; --j) {
                                    Packet* layer = copy_stack.top();
                                    if (layer->get_layer_ID() == 0) {
                                        appLayerPacket = dynamic_cast<ApplicationLayerPacket*>(layer);
                                        if (appLayerPacket) {
                                            carriedMessage = appLayerPacket->get_message_data();
                                            print0 = "Sender ID: " + appLayerPacket->get_sender_ID() + ", Receiver ID: " + appLayerPacket->get_receiver_ID();
                                        senderId = appLayerPacket->get_sender_ID();
                                        }
                                    } else if (layer->get_layer_ID() == 1) {
                                        transLayerPacket = dynamic_cast<TransportLayerPacket*>(layer);

                                    } else if (layer->get_layer_ID() == 2) {
                                        netLayerPacket = dynamic_cast<NetworkLayerPacket*>(layer);

                                    } else if (layer->get_layer_ID() == 3) {
                                        physLayerPacket = dynamic_cast<PhysicalLayerPacket*>(layer);

                                    }
                                    if(!copy_stack.empty())
                                        copy_stack.pop();

                                }
                            }
                        }
                    } else {
                        cout<<"No such frame."<<endl;
                        break;
                    }
                }
            }
            for (auto& cli : clients) {
                if (cli.getClientId() == senderId) {
                    hopNumber = cli.number_of_hops;
                }
            }
            if (outExist && statusType == "out" || inExist && statusType == "in") {
                string status_type2;
                if (statusType == "out")
                    status_type2 = "outgoing";
                else if (statusType == "in")
                    status_type2 = "incoming";

                cout << "Current Frame #" << frameNumber << " on the " << status_type2 << " queue of client " << clientName << endl;
                cout << "Carried Message: \"" << carriedMessage << "\"" << endl;
                cout<<"Layer "<< appLayerPacket->get_layer_ID()<< " info: ";cout<<print0<<endl;
                cout<<"Layer "<< transLayerPacket->get_layer_ID()<< " info: "; transLayerPacket->print();
                cout<<"Layer "<< netLayerPacket->get_layer_ID()<< " info: ";   netLayerPacket->print();
                cout<<"Layer "<< physLayerPacket->get_layer_ID()<< " info: ";physLayerPacket->print();
                cout << "Number of hops so far: "<<hopNumber<< endl;
            }

        } else if (command.substr(0, 11) == "SHOW_Q_INFO") {
            string line = command;
            istringstream iss(line);
            vector<string> tokens;

            // Split the line into tokens based on spaces
            while (iss >> line) {
                tokens.push_back(line);
            }
            string clientName = tokens[1];
            string statusType = tokens[2];

            if (statusType == "out") {
                for (auto& client : clients) {
                    if (client.getClientId() == clientName) {
                        cout<<"Client "<<clientName<<" Outgoing Queue Status"<<endl;
                        size_t queueSize = client.get_outgoing_queue().size();
                        cout<<"Current total number of frames: "<<queueSize<<endl;
                    }
                }
            } else if (statusType == "in") {
                for (auto& client : clients) {
                    if (client.getClientId() == clientName) {
                        cout<<"Client "<<clientName<<" Incoming Queue Status"<<endl;
                        size_t queueSize = client.get_incoming_queue().size();
                        cout<<"Current total number of frames: "<<queueSize<<endl;
                    }
                }
            }

        } else if (command.substr(0, 4) == "SEND") {

            string receiverId, senderId, recent_receiver_mac, recent_sender_mac, recent_receiver_id, recent_sender_id;
            int hopNumber;
            for (auto& client : clients) {

                if (!client.outgoing_queue.empty()) {


                    ApplicationLayerPacket* appLayerPacket;
                    TransportLayerPacket* transLayerPacket;
                    NetworkLayerPacket* netLayerPacket;
                    PhysicalLayerPacket* physLayerPacket;


                    size_t size_of_queue = client.outgoing_queue.size();
                    stack<Packet*> copy_stack;
                    stack<Packet*> new_stack;
                    int check_hop = 0;
                    for (int i = 1; i <= size_of_queue; ++i) {

                        if (!client.outgoing_queue.empty()) {
                            copy_stack = client.outgoing_queue.front();

                        }

                        for (int j = 3; j >= 0; --j) {
                            Packet* layer = copy_stack.top();
                            if (layer->get_layer_ID() == 0) {
                                appLayerPacket = dynamic_cast<ApplicationLayerPacket*>(layer);
                                senderId = appLayerPacket->get_sender_ID();

                            } else if (layer->get_layer_ID() == 1) {
                                transLayerPacket = dynamic_cast<TransportLayerPacket*>(layer);

                            } else if (layer->get_layer_ID() == 2) {
                                netLayerPacket = dynamic_cast<NetworkLayerPacket*>(layer);

                            } else if (layer->get_layer_ID() == 3) {
                                physLayerPacket = dynamic_cast<PhysicalLayerPacket*>(layer);
                                if (physLayerPacket) {
                                    recent_receiver_mac = physLayerPacket->get_receiver_MAC_address();
                                    recent_sender_mac = physLayerPacket->get_sender_MAC_address();
                                }
                            }
                            if (!copy_stack.empty())
                                copy_stack.pop();

                        }
                        new_stack.push(appLayerPacket);
                        new_stack.push(transLayerPacket);
                        new_stack.push(netLayerPacket);
                        new_stack.push(physLayerPacket);

                        for (auto& cli : clients) {

                            if (cli.getClientMac() == recent_receiver_mac) {
                                recent_receiver_id = cli.getClientId();
                                cli.incoming_queue.push(new_stack);
                            } if (cli.getClientMac() == recent_sender_mac) {
                                recent_sender_id = cli.getClientId();
                            } if (cli.getClientId() == senderId) {
                                check_hop++;
                                if (check_hop == 1) {
                                    cli.number_of_hops++;
                                    hopNumber = cli.number_of_hops;
                                }


                            }
                        }

                        cout<<"Client "<<recent_sender_id<<" sending frame #"<<i<<" to client #"<<recent_receiver_id<<endl;
                        physLayerPacket->print();netLayerPacket->print();transLayerPacket->print();appLayerPacket->print();
                        cout<<"Number of hops so far: "<<hopNumber<<endl;
                        cout<<string(client.getClientId().length() + 7, '-')<<endl;
                        cout<<"why"<<endl;
                        status[senderId] = "Message Sent";
                        if (!client.outgoing_queue.empty()) {
                            client.outgoing_queue.pop();
                        }


                    }

                }
            }

        } else if (command.substr(0, 7) == "RECEIVE") {
            for (auto& client : clients) {
                if (!client.incoming_queue.empty()) {
                    string receiverId, senderId, past_receiver_mac, past_sender_mac, past_receiver_id, past_sender_id;
                    string new_sender_mac, new_sender_id, new_receiver_mac, new_receiver_id;

                    int hopNumber;

                    size_t size_of_queue = client.incoming_queue.size();


                    stack<Packet *> copy_stack;

                    copy_stack = client.incoming_queue.front();
                    string full_message;
                    int count_print = 0;
                    int count_forward = 0;
                    bool last_destination = false;
                    int check_frame = 0;

                    for (int i = 1; i <= size_of_queue; ++i) {


                        ApplicationLayerPacket *appLayerPacket;
                        TransportLayerPacket *transLayerPacket;
                        NetworkLayerPacket *netLayerPacket;
                        PhysicalLayerPacket *physLayerPacket;
                        stack<Packet *> forward_stack;

                        if (!client.incoming_queue.empty()) {
                            copy_stack = client.incoming_queue.front();

                        }

                        for (int j = 3; j >= 0; --j) {

                            Packet *layer = copy_stack.top();
                            if (layer->get_layer_ID() == 0) {
                                appLayerPacket = dynamic_cast<ApplicationLayerPacket *>(layer);
                                if (appLayerPacket) {
                                    receiverId = appLayerPacket->get_receiver_ID();
                                    senderId = appLayerPacket->get_sender_ID();
                                    full_message += appLayerPacket->get_message_data();

                                }
                            } else if (layer->get_layer_ID() == 1) {
                                transLayerPacket = dynamic_cast<TransportLayerPacket *>(layer);

                            } else if (layer->get_layer_ID() == 2) {
                                netLayerPacket = dynamic_cast<NetworkLayerPacket *>(layer);

                            } else if (layer->get_layer_ID() == 3) {
                                physLayerPacket = dynamic_cast<PhysicalLayerPacket *>(layer);
                                if (physLayerPacket) {
                                    past_receiver_mac = physLayerPacket->get_receiver_MAC_address();
                                    past_sender_mac = physLayerPacket->get_sender_MAC_address();

                                }
                            }
                            if (!copy_stack.empty()) {
                                copy_stack.pop();

                            }

                        }
                        forward_stack.push(appLayerPacket);
                        forward_stack.push(transLayerPacket);
                        forward_stack.push(netLayerPacket);
                        forward_stack.push(physLayerPacket);


                        for (auto &cli: clients) {
                            if (cli.getClientId() == senderId) {
                                hopNumber = cli.number_of_hops;
                            }
                            if (cli.getClientMac() == past_receiver_mac) {
                                past_receiver_id = cli.getClientId();
                            }
                            if (cli.getClientMac() == past_sender_mac) {
                                past_sender_id = cli.getClientId();
                            }
                        }


                        if (client.getClientId() == receiverId) {
                            last_destination = true;

                            cout << "Client " << past_receiver_id << " receiving frame #" << i << " from client, "
                                 << past_sender_id << ", originating from client " << senderId << endl;
                            physLayerPacket->print();
                            netLayerPacket->print();
                            transLayerPacket->print();
                            appLayerPacket->print();
                            cout << "Number of hops so far: " << hopNumber << endl;
                            cout << string(client.getClientId().length() + 7, '-') << endl;


                        } else {


                            for (auto &route: client.routing_table) {

                                if (route.first == receiverId) {
                                    //   int check_frame = 0;
                                    for (auto &cli: clients) {

                                        if (route.second == cli.getClientId()) {

                                            new_receiver_id = cli.getClientId();
                                            new_receiver_mac = cli.getClientMac();

                                            new_sender_mac = client.getClientMac();
                                            new_sender_id = client.getClientId();
                                            physLayerPacket->set_sender_MAC_address(past_receiver_mac);
                                            physLayerPacket->set_receiver_MAC_address(new_receiver_mac);
                                            for (auto &clie: clients) {
                                                if (clie.getClientId() == new_receiver_id) {
                                                    clie.outgoing_queue.push(forward_stack);
                                                }
                                            }
                                            count_forward++;
                                            if (count_forward == 1) {
                                                //cout<<"Client "<<past_receiver_id<<" frame #"<<i<<" from client "<<past_sender_id<<", but intended for client "<<receiverId<<". Forwarding..."<<endl;
                                                cout << "Client " << past_receiver_id
                                                     << " receiving a message from client " << past_sender_id
                                                     << ", but intended for client " << receiverId << ". Forwarding... "
                                                     << endl;
                                                status[past_receiver_id] = "Message Forwarded";
                                            }
                                            cout << "Frame #" << i << " MAC address change: New sender MAC "
                                                 << new_sender_mac << ", new receiver MAC " << new_receiver_mac << endl;
                                            break;
                                        } else {
                                            check_frame++;

                                            if (check_frame == size_of_queue) {
                                                cout << "Client " << past_receiver_id
                                                     << " receiving a message from client " << past_sender_id
                                                     << ", but intended for client " << receiverId << ". Forwarding... "
                                                     << endl;

                                                cout << "Error: Unreachable destination. Packets are dropped after "
                                                     << hopNumber << " hops!" << endl;
                                                status[past_receiver_id] = "Message Dropped";
                                            }

                                        }
                                    }
                                }
                            }

                        }
                        if (!client.incoming_queue.empty()) {
                            client.incoming_queue.pop();
                        }

                    }
                    if (last_destination) {
                        cout << "Client " << receiverId << " received the message \"" << full_message
                             << "\" from client " << senderId << "." << endl;
                        status[receiverId] = "Message Received";
                    }
                    for (auto &xx: clients) {
                        if (xx.getClientId() == senderId) {
                            xx.number_of_hops = 0;
                        }
                    }

                }
            }


        } else if (command.substr(0, 9) == "PRINT_LOG") {
            ActivityType activity_type;
            bool success_status;
            string timestamp;

            int number_of_hops;

            string client;
            string line = command;
            istringstream iss(line);

            (iss >> line >> client) ;
            Log log(timestamp, message_content, number_of_frames, number_of_hops, sender_id, receiver_id, success_status, activity_type);
            for (auto& statu : status) {
                cout<<"cout: "<<statu.first<<", "<<statu.second<<endl;
                if (statu.first == client) {
                    ActivityType.M
                    Log logEntry("2023-11-22 20:30:03", message_content, number_of_frames, number_of_hops, sender_id, receiver_id, success_status, ActivityType::statu.second);

                    cout << "Timestamp: " << log.getTimestamp() << ", "
                         << "Message Content: " << log.getMessageContent() << ", "
                         << "Number of Frames: " << log.getNumberOfFrames() << ", "
                         << "Number of Hops: " << log.getNumberOfHops() << ", "
                         << "Sender ID: " << log.getSenderId() << ", "
                         << "Receiver ID: " << log.getReceiverId() << ", "
                         << "Success Status: " << (log.getSuccessStatus() ? "true" : "false") << ", "
                         << "Activity Type: " << log.getActivityType() << endl;

                }
            }


        } else {
            cout<<"Invalid command."<<endl;
        }
    }
}

vector<Client> Network::read_clients(const string &filename) {
    // TODO: Read clients from the given input file and return a vector of Client instances.

    ifstream file(filename);
    string clientNumberStr;

    if (file.is_open()) {
        getline(file, clientNumberStr);
        int clientNumber = stoi(clientNumberStr);
        for (int i = 0; i < clientNumber; ++i) {

            string line;
            if (getline(file, line)) {
                istringstream iss(line);
                vector<string> tokens;

                // Split the line into tokens based on spaces
                while (iss >> line) {
                    tokens.push_back(line);
                }
                if (tokens.size() >= 3) {
                    string client_id = tokens[0];
                    string client_ip = tokens[1];
                    string client_mac = tokens[2];

                    // Create a Client object and add it to the vector
                    clients.emplace_back(client_id, client_ip, client_mac);
                } else {
                    cerr << "Error processing line " << i + 1 << ": Insufficient tokens." << endl;
                }

            } else {
                std::cerr << "Error reading line " << i + 1 << " from the file." << std::endl;
                break;  // exit the loop if an error occurs
            }
        }

        file.close();
    } else {
        ostringstream errorMsg;
        errorMsg << "Cannot open the file " << filename;
        cout << errorMsg.str() << endl;
    }


    return clients;
}

void Network::read_routing_tables(vector<Client> &clients, const string &filename) {
    // TODO: Read the routing tables from the given input file and populate the clients' routing_table member variable.
    ifstream file(filename);
    string line;
    int count = 0;
    if (file.is_open()) {
        while (getline(file, line)) {
            istringstream iss(line);
            vector<string> tokens;
            if (line == "-") {
                count++;

            }

            // Split the line into tokens based on spaces
            while (iss >> line) {
                tokens.push_back(line);
            }
            if (tokens.size() == 2) {
                string receiverId = tokens[0];
                string nextHopId = tokens[1];
                if (count < clients.size()) {
                    clients[count].routing_table.insert(make_pair(receiverId, nextHopId));
                }

            }
        }

        file.close();

    } else {
        ostringstream errorMsg;
        errorMsg << "Cannot open the file " << filename;
        cout << errorMsg.str() << endl;
    }


}

// Returns a list of token lists for each command
vector<string> Network::read_commands(const string &filename) {
    // TODO: Read commands from the given input file and return them as a vector of strings.

    vector<string> commands;
    int commandNumber;
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        getline(file,line);
        commandNumber = stoi(line);
        while (getline(file, line)) {
            commands.push_back(line);
        }
        file.close();
    } else {
        ostringstream errorMsg;
        errorMsg << "Cannot open the file " << filename;
        cout << errorMsg.str() << endl;
    }
    return commands;
}

Network::~Network() {
    // TODO: Free any dynamically allocated memory if necessary.
    while (!frame.empty()) {
        Packet* x = frame.top();

        frame.pop();
        delete x;
    }

}



