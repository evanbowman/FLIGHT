#include <FLIGHT/Core/LoadBlueprints.hpp>

namespace FLIGHT {
inline static Blueprint ReadPartsList(pugi::xml_node plane) {
    Blueprint blueprint;
    for (auto part : plane) {
	Blueprint::Part p;
        auto material = part.attribute("material");
	auto model = part.attribute("model");
	auto texture = part.attribute("texture");
	auto position = part.child("Position");
	auto scale = part.child("Scale");
	auto rotation = part.child("Rotation");
	if (not material or not model or not texture) {
	    std::string id_str;
	    auto id_node = part.attribute("id");
	    if (id_node) {
		id_str = id_node.value();
	    }
	    throw std::runtime_error("Part \'" + id_str +
				     "\' missing required attribute");
	}
	p.material = material.value();
	p.model = model.value();
	p.texture = texture.value();
	if (position) {
	    auto x = position.attribute("x");
	    auto y = position.attribute("y");
	    auto z = position.attribute("z");
	    if (x) p.position.x = x.as_float();
	    if (y) p.position.y = y.as_float();
	    if (z) p.position.z = z.as_float();
	}
	if (scale) {
	    auto x = scale.attribute("x");
	    auto y = scale.attribute("y");
	    auto z = scale.attribute("z");
	    if (x) p.scale.x = x.as_float();
	    if (y) p.scale.y = y.as_float();
	    if (z) p.scale.z = z.as_float();
	}
	if (rotation) {
	    auto x = rotation.attribute("x");
	    auto y = rotation.attribute("y");
	    auto z = rotation.attribute("z");
	    if (x) p.rotation.x = glm::radians(x.as_float());
	    if (y) p.rotation.y = glm::radians(y.as_float());
	    if (z) p.rotation.z = glm::radians(z.as_float());
	}
	blueprint.AddPart(p);
    }
    return blueprint;
}

PlaneRegistry LoadPlanes() {
    pugi::xml_document doc;
    auto res =
        doc.load_file((ResourcePath() + "blueprints/planes.xml").c_str());
    if (not res) {
	throw std::runtime_error("Invalid format in planes.xml");
    }
    PlaneRegistry registry;
    for (auto & plane : doc.child("Planes")) {
        auto id = plane.attribute("id");
	if (not id) {
	    throw std::runtime_error("Plane missing id tag");
	}
	registry[id.value()] = ReadPartsList(plane);
    }
    return registry;
}
}
