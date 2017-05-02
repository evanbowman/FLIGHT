#include <FLIGHT/Core/Game.hpp>
#include <FLIGHT/Core/Serialize.hpp>

namespace FLIGHT {
size_t XMLSerializer::GetUID(void * addr) {
    auto node = m_idTable.find(addr);
    if (node not_eq m_idTable.end()) {
        return node->second;
    }
    // This skips UID 0. It's reserved for NULL reference ids.
    m_idTable[addr] = ++m_maxUID;
    return m_maxUID;
}

XMLSerializer::XMLSerializer() : m_maxUID(0) {
    m_roots.push(m_doc.append_child("Data"));
}

void XMLSerializer::PushGroup(const std::string & name) {
    m_roots.push(m_roots.top().append_child(name.c_str()));
}

void XMLSerializer::PopGroup() {
    m_roots.pop();
    assert(not m_roots.empty());
}

void XMLSerializer::Dispatch(const Player & player) {
    auto p = m_roots.top().append_child("Player");
    p.append_child("Score")
        .append_child(pugi::node_pcdata)
        .set_value(std::to_string(player.GetScore()).c_str());
    p.append_child("Plane").append_attribute("refid") =
        std::to_string(GetUID(player.GetPlane().get())).c_str();
}

void XMLSerializer::Dispatch(const Plane & plane) {
    auto p = m_roots.top().append_child("Plane");
    p.append_attribute("id") = std::to_string(GetUID((void *)&plane)).c_str();
    p.append_attribute("class") = plane.GetBlueprintName().c_str();
    const auto & position = plane.GetPosition();
    auto pos = p.append_child("Position");
    pos.append_child("x")
        .append_child(pugi::node_pcdata)
        .set_value(std::to_string(position.x).c_str());
    pos.append_child("y")
        .append_child(pugi::node_pcdata)
        .set_value(std::to_string(position.y).c_str());
    pos.append_child("z")
        .append_child(pugi::node_pcdata)
        .set_value(std::to_string(position.z).c_str());
    const auto & rotation = plane.GetRotation();
    auto rot = p.append_child("Rotation");
    rot.append_child("x")
        .append_child(pugi::node_pcdata)
        .set_value(std::to_string(rotation.x).c_str());
    rot.append_child("y")
        .append_child(pugi::node_pcdata)
        .set_value(std::to_string(rotation.y).c_str());
    rot.append_child("z")
        .append_child(pugi::node_pcdata)
        .set_value(std::to_string(rotation.z).c_str());
}

void XMLSerializer::Dispatch(const Coin &) {
    // ...
}

void XMLSerializer::Dispatch(const Game & game) {
    auto header = m_roots.top().append_child("Header");
    header.append_child("Seed")
        .append_child(pugi::node_pcdata)
        .set_value(std::to_string(game.GetSeed()).c_str());
}

void XMLSerializer::Dump(std::ostream & stream) { m_doc.save(stream, "  "); }
}
