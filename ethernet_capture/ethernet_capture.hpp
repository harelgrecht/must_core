#pragma once

#include <string>
#include <vector>
#include "pcap.h"
#include "../ThreadSafeQueue/threadSafeQueue.hpp"
#include "../EthDevice/EthDevice.hpp"
#include "../third_party/nlohmann/json.hpp"
#include "../LogHandler/logger.hpp"

struct PcapSettings {
    int packetSize;
    int promiscMode;
    int captureTimeoutMs;
    int captureLoopCount;
    std::string filter;
};

template <typename T>
class EthernetCapture {
public:
    EthernetCapture(ThreadSafeQueue<T>& receiveQueue, const std::string& pcapFilter, const EthDevice& ethDevice);
    ~EthernetCapture();

    void startCapture();

private:
    ThreadSafeQueue<T>& receiveQueue_;
    EthDevice ethDevice_;
    std::string pcapFilter_;
    PcapSettings settings_;
    char errorBuffer_[PCAP_ERRBUF_SIZE];

    T packetData_;
    std::unique_ptr<pcap_t, decltype(&pcap_close)> pcapHandle_{nullptr, &pcap_close};

    void loadPcapConfig();
    void applyPcapSettings();
    void receivePacketHandler(uint8_t* globalData, const struct pcap_pkthdr* packetHeader);
};

template <typename T>
EthernetCapture<T>::EthernetCapture(ThreadSafeQueue<T>& receiveQueue, const std::string& pcapFilter, const EthDevice& ethDevice)
    : receiveQueue_(receiveQueue), pcapFilter_(pcapFilter), ethDevice_(ethDevice) { }

template <typename T>
EthernetCapture<T>::~EthernetCapture() {
    pcapHandle_.reset();
}

template <typename T>
void EthernetCapture<T>::applyPcapSettings() {
    pcapHandle_.reset(pcap_open_live(ethDevice_.getName().c_str(), settings_.packetSize, settings_.promiscMode, settings_.captureTimeoutMs, errorBuffer_));

    if (!pcapHandle_) {
        Logger::getInstance().Log(LogLevel::ERROR, "Failed to open device: " + std::string(errorBuffer_));
        throw std::runtime_error("Failed to open live device.");
    }

    struct bpf_program bpfFilter;
    if (pcap_compile(pcapHandle_.get(), &bpfFilter, pcapFilter_.c_str(), 0, PCAP_NETMASK_UNKNOWN) == -1) {
        Logger::getInstance().Log(LogLevel::ERROR, "Failed to compile filter: " + std::string(pcap_geterr(pcapHandle_.get())));
        return;
    }

    if (pcap_setfilter(pcapHandle_.get(), &bpfFilter) == -1) {
        Logger::getInstance().Log(LogLevel::ERROR, "Failed to set filter: " + std::string(pcap_geterr(pcapHandle_.get())));
        pcap_freecode(&bpfFilter);
    }

    Logger::getInstance().Log(LogLevel::INFO, "PCAP filter applied: " + pcapFilter_);
    pcap_freecode(&bpfFilter);
}

template <typename T>
void EthernetCapture<T>::startCapture() {
    applyPcapSettings();
    Logger::getInstance().Log(LogLevel::INFO, "Starting capture on: " + ethDevice_.getName());

    if (pcap_loop(pcapHandle_.get(), settings_.captureLoopCount, receivePacketHandler, reinterpret_cast<uint8_t*>(this)) < 0) {
        Logger::getInstance().Log(LogLevel::ERROR, "Error capturing packets: " + std::string(pcap_geterr(pcapHandle_.get())));
    }
}

template <typename T>
void EthernetCapture<T>::receivePacketHandler(uint8_t* globalData, const struct pcap_pkthdr* packetHeader) {
    receiveQueue_.enqueue(packetData_);
}
