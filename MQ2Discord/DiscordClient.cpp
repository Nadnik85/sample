#include <sleepy_discord\websocketpp_websocket.h>
#include <nlohmann/json.hpp>

#include "SafeQueue.h"

// Queue for messages to be sent to discord channel
SafeQueue<std::string> toDiscord;

// Queue for messages to be displayed in chat
SafeQueue<std::string> fromDiscord;

std::thread * g_pThread;
std::atomic_bool bRun(false);
std::atomic_bool bFinished(false);

std::string escape_json(const std::string &s) {
	std::ostringstream o;
	for (auto c = s.cbegin(); c != s.cend(); c++) {
		if (*c == '"' || *c == '\\' || ('\x00' <= *c && *c <= '\x1f')) {
			o << "\\u"
				<< std::hex << std::setw(4) << std::setfill('0') << (int)*c;
		}
		else {
			o << *c;
		}
	}
	return o.str();
}

// Real simple client, all it does is invoke a callback when a message is received
class CallbackDiscordClient : public SleepyDiscord::DiscordClient {
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	CallbackDiscordClient(const std::string token, std::function<void(SleepyDiscord::Message &)> callback)
		: SleepyDiscord::DiscordClient(token, 2), // 0 is numThreads, doesn't do anything if < 3
		_callback(callback)
	{
	}

	void onMessage(SleepyDiscord::Message message) {
		if (_callback)
			_callback(message);
	}
private:
	std::function<void(SleepyDiscord::Message &)> _callback;
};

void DiscordThread(std::string token, std::string channelId, std::string controlUserId, std::string connectMsg)
{
	try
	{
		CallbackDiscordClient client(token, [&](SleepyDiscord::Message & msg) {
			if (msg.author.ID == controlUserId && msg.channelID == channelId)
				fromDiscord.enqueue(msg.content);
		});

		fromDiscord.enqueue(connectMsg);

		unsigned int loopCount = 0;
		while (bRun)
		{
			// Poll to receive messages
			client.poll();

			// Send messages from queue, combine them if there's multiple as the API has rate limiting
			std::string msg, combinedMsg;
			while (combinedMsg.length() <= 1800 && toDiscord.tryDequeue(msg))
			{
				combinedMsg += msg + "\n";
			}

			if (combinedMsg != "")
			{
				auto response = client.sendMessage(channelId, escape_json(combinedMsg));
				if (response.statusCode != 200)
					fromDiscord.enqueue("Error, response text: " + response.text);
			}

			// Every minute, send typing, to keep connection alive. Crude timer based on 1s sleep below
			if (loopCount++ % 60 == 0)
				client.sendTyping(channelId);

			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1000ms);
		}

		fromDiscord.enqueue("Disconnected");
	}
	catch (std::exception & e)
	{
		fromDiscord.enqueue(std::string("Exception: ") + e.what());
	}

	bFinished = true;
}