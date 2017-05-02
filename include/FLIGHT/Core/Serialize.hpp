#pragma once

#include <ostream>
#include <pugixml.hpp>
#include <cassert>
#include <stack>
#include <unordered_map>

namespace FLIGHT {
    class Plane;
    class Game;
    class Coin;
    class Player;
    class Camera;
    
    class Serializer {
    public:
	virtual void Dispatch(const Plane & plane) = 0;
	virtual void Dispatch(const Player & player) = 0;
	virtual void Dispatch(const Game & game) = 0;
	virtual void Dispatch(const Coin & coin) = 0;
	virtual void Dump(std::ostream & stream) = 0;
    };
    
    class Serializable {
    public:
	virtual void Serialize(Serializer & serializer) = 0;
	virtual ~Serializable() {};
    };

    class XMLSerializer : public Serializer {
	pugi::xml_document m_doc;
	std::stack<pugi::xml_node> m_roots;
	std::unordered_map<void *, size_t> m_idTable;
	size_t m_maxUID;
	size_t GetUID(void * addr);
    public:
	XMLSerializer();
	void Dispatch(const Plane & plane) override;
	void Dispatch(const Game & game) override;
	void Dispatch(const Player & player) override;
	void Dispatch(const Coin & coin) override;
	void Dump(std::ostream & stream) override;
	void PushGroup(const std::string & name);
	void PopGroup();
    };

    class ProtobufSerializer : public Serializer {
    public:
	// TODO...
    };
}
