//
// Created by Aleksey Timin on 11/16/19.
//

#include <sstream>
#include <cip/connectionManager/NetworkConnectionParams.h>
#include "SessionInfo.h"
#include "ConnectionManager.h"
#include "utils/Logger.h"
#include "utils/Buffer.h"
#include <stdio.h>
#include <unistd.h>

using namespace eipScanner::cip;
using eipScanner::SessionInfo;
using eipScanner::MessageRouter;
using eipScanner::ConnectionManager;
using eipScanner::IOConnection;
using eipScanner::cip::connectionManager::ConnectionParameters;
using eipScanner::cip::connectionManager::NetworkConnectionParams;
using eipScanner::utils::Buffer;
using eipScanner::utils::Logger;
using eipScanner::utils::LogLevel;

int main() {
  Logger::setLogLevel(LogLevel::DEBUG);

  auto si = std::make_shared<SessionInfo>("192.168.1.26", 0xAF12);

  // Implicit messaging
  ConnectionManager connectionManager;

  ConnectionParameters parameters;
  parameters.connectionPath = {0x20, 0x04, 0x24, 0x97, 0x2C, 0x96, 0x2C, 0x64};  // config Assm151, output Assm150, intput Assm100
  parameters.o2tRealTimeFormat = true;
  parameters.originatorVendorId = 1;
  parameters.originatorSerialNumber = 32423;
  parameters.t2oNetworkConnectionParams |= NetworkConnectionParams::P2P;
  parameters.t2oNetworkConnectionParams |= NetworkConnectionParams::SCHEDULED_PRIORITY;
  parameters.t2oNetworkConnectionParams |= 32; //size of Assm100 =32
  parameters.o2tNetworkConnectionParams |= NetworkConnectionParams::P2P;
  parameters.o2tNetworkConnectionParams |= NetworkConnectionParams::SCHEDULED_PRIORITY;
  parameters.o2tNetworkConnectionParams |= 32; //size of Assm150 = 32

  parameters.originatorSerialNumber = 0x12345;
  parameters.o2tRPI = 1000 * 1000;
  parameters.t2oRPI = 1000 * 1000;
  parameters.transportTypeTrigger |= NetworkConnectionParams::CLASS1;

  IOConnection::WPtr io = connectionManager.forwardOpenWithIOParams(
          si, parameters,
          137625620, 3046309889, 1000000, 1000000, 1, 50);
  auto ptr = io.lock();
  ptr->print();

  if (ptr) {
    ptr->setReceiveDataListener([](auto realTimeHeader, auto sequence, auto data) {
      printf("R: ");
      for (auto &byte : data) {
        printf("[%x]", byte);
      }
      printf("\n");
    });

    ptr->setCloseListener([]() {
      printf("Listener Closed\n");
    });
  } else {
      exit(1);
  }

  int count = 100000;
  std::vector<uint8_t> data(32);
  while (connectionManager.hasOpenConnections() && count-- > 0) {
    data[1] += 1;
    // printf("S: [%x]\n", data[0]);
    // printf("S: ");
    // for (auto &byte: data) {
    //   printf("[%x]", byte);
    // }
    // printf("\n");
    ptr->setDataToSend(data);
    printf("----------------------------------------------------------\n");
    connectionManager.handleConnections(std::chrono::milliseconds(0));
    ptr->print_o2tSequenceNumber();
    sleep(1);
  }

  connectionManager.forwardClose(si, io);

  return 0;
}
