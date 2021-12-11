/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2021 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include <rmxbase.h>


class Sockets
{
public:
	static void startupSockets();
	static void shutdownSockets();
};


struct SocketAddress
{
public:
	inline SocketAddress() :
		mHasSockAddr(false),
		mHasIpPort(false),
		mPort(0)
	{}

	inline SocketAddress(const std::string& ip, uint16 port) :
		mHasSockAddr(false),
		mHasIpPort(true),
		mIP(ip),
		mPort(port)
	{}

	inline const uint8* getSockAddr() const  { assureSockAddr();  return mSockAddr; }
	inline uint8* accessSockAddr() const	 { return mSockAddr; }

	inline const std::string& getIP() const	 { assureIpPort();  return mIP; }
	inline uint16 getPort() const			 { assureIpPort();  return mPort; }
	inline std::string toString() const		 { assureIpPort();  return mIP + ':' + std::to_string(mPort); }

	inline bool isValid() const  { return (mHasSockAddr || mHasIpPort); }

	inline void set(const std::string& ip, uint16 port)
	{
		mHasSockAddr = false;
		mHasIpPort = true;
		mIP = ip;
	#ifdef _MSC_VER
		mIP = ip;	// Two assignments because there seems to be a weird compiler error that makes the first assignment not work
	#endif
		mPort = port;
	}

	inline void set(const uint8* sockAddr)
	{
		memcpy(mSockAddr, sockAddr, 128);
		mHasSockAddr = true;
		mHasIpPort = false;
	}

	inline void onSockAddrSet()
	{
		mHasSockAddr = true;
		mHasIpPort = false;
	}

	inline void writeTo(std::string& outIP, uint16& outPort) const
	{
		assureIpPort();
		outIP = mIP;
		outPort = mPort;
	}

	uint64 getHash() const;

	void assureSockAddr() const;
	void assureIpPort() const;

	bool operator==(const SocketAddress& other) const;
	inline bool operator!=(const SocketAddress& other) const { return !operator==(other); }

private:
	// Yes, everything is mutable here indeed...
	mutable bool mHasSockAddr = false;
	mutable bool mHasIpPort = false;
	mutable uint8 mSockAddr[128];	// Size of sockaddr_storage (pretty large, huh?)
	mutable std::string mIP;		// Usually an IPv4 as string, e.g. "123.45.67.89"
	mutable uint16 mPort = 0;
};


class TCPSocket
{
public:
	struct ReceiveResult
	{
		std::vector<uint8> mBuffer;
	};

public:
	~TCPSocket();

	bool isValid() const;

	// TODO: Add "setupServer" for TCP server
	bool connectTo(const std::string& serverAddress, uint16 serverPort);
	void close();

	bool sendData(const uint8* data, size_t length);

	bool receiveBlocking(ReceiveResult& outReceiveResult);

private:
	struct Internal;
	Internal* mInternal = nullptr;
};


class UDPSocket
{
public:
	struct ReceiveResult
	{
		std::vector<uint8> mBuffer;
		SocketAddress mSenderAddress;
	};

public:
	~UDPSocket();

	bool isValid() const;

	bool bindToPort(uint16 port);
	bool bindToAnyPort();
	void close();

	bool sendData(const uint8* data, size_t length, const SocketAddress& destinationAddress);
	bool sendData(const std::vector<uint8>& data, const SocketAddress& destinationAddress);

	bool receiveBlocking(ReceiveResult& outReceiveResult);
	bool receiveNonBlocking(ReceiveResult& outReceiveResult);

private:
	bool receiveInternal(ReceiveResult& outReceiveResult);

private:
	struct Internal;
	Internal* mInternal = nullptr;
};