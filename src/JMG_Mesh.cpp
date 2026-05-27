#include "../JMGraphics.h"

JMGraphics::Mesh::Mesh(JMGraphics* Parent) {
	parent = Parent;
	glGenVertexArrays(1, &meshVAO);
	glGenBuffers(1, &meshVBO);
	glGenBuffers(1, &meshEBO);
}

JMGraphics::Mesh::~Mesh() {
	glDeleteVertexArrays(1, &meshVAO);
	glDeleteBuffers(1, &meshVBO);
	glDeleteBuffers(1, &meshEBO);
}

void JMGraphics::Mesh::display() {
	if (vertices.empty()) {
		return;
	}

	if (reProcess) {
		process();
		reProcess = false;
	}

	parent->activateShader(parent->shaderMesh);
	parent->shaderMesh->setMat4("transform", parent->tranformMat);

	glBindVertexArray(meshVAO);
	glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);

	//position pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//texCoord pointer
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//color pointer
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	//normal pointer
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	if (renderFaces) {
		glDrawElements(GL_TRIANGLES, renderVertNum, GL_UNSIGNED_INT, 0);
	}
	if (renderWireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		parent->shaderMesh->setBool("wireframeMode", true);
		parent->shaderMesh->set4Float("wireframeColor", wireframeColor);
		glDrawElements(GL_TRIANGLES, renderVertNum, GL_UNSIGNED_INT, 0);
		parent->shaderMesh->setBool("wireframeMode", false);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	

	glBindVertexArray(0);
}
void JMGraphics::Mesh::process() {
	if (vertices.empty()) {
		return;
	}

	std::vector<float> data;
	for (int i = 0; i < vertices.size(); i++) {
		data.push_back(vertices[i].pos.x);
		data.push_back(vertices[i].pos.y);
		data.push_back(vertices[i].pos.z);

		data.push_back(vertices[i].texCoord.x);
		data.push_back(vertices[i].texCoord.y);

		data.push_back(vertices[i].color.x);
		data.push_back(vertices[i].color.y);
		data.push_back(vertices[i].color.z);
		data.push_back(vertices[i].color.w);

		data.push_back(vertices[i].normal.x);
		data.push_back(vertices[i].normal.y);
		data.push_back(vertices[i].normal.z);
	}

	glBindVertexArray(meshVAO);

	glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	renderVertNum = (unsigned int)indices.size();
}

JMGraphics::Mesh* JMGraphics::Mesh::clone() const{
	JMGraphics::Mesh* c = new JMGraphics::Mesh(this->parent);
	c->name = name;
	c->renderFaces = renderFaces;
	c->renderWireframe = renderWireframe;
	c->wireframeColor = wireframeColor;
	for (int i = 0; i < vertices.size(); i++) {
		c->vertices.push_back(vertices[i]);
	}
	for (int i = 0; i < indices.size(); i++) {
		c->indices.push_back(indices[i]);
	}
	for (int i = 0; i < textures.size(); i++) {
		c->textures.push_back(textures[i]);
	}
	c->process();
	return c;
}


void JMGraphics::Mesh::writeData(std::ostream& stream) {
	stream << "MESH_DATA\n";
	stream << vertices.size() << '\n';

	std::vector<float> data;
	for (int i = 0; i < vertices.size(); i++) {
		data.push_back(vertices[i].pos.x);
		data.push_back(vertices[i].pos.y);
		data.push_back(vertices[i].pos.z);

		data.push_back(vertices[i].texCoord.x);
		data.push_back(vertices[i].texCoord.y);

		data.push_back(vertices[i].color.x);
		data.push_back(vertices[i].color.y);
		data.push_back(vertices[i].color.z);
		data.push_back(vertices[i].color.w);

		data.push_back(vertices[i].normal.x);
		data.push_back(vertices[i].normal.y);
		data.push_back(vertices[i].normal.z);
	}

	stream.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size() * sizeof(float)));

	stream << '\n';

	stream << indices.size() << '\n';

	for (int i = 0; i < indices.size(); i++) {
		stream << indices[i] << ' ';
	}

	stream << '\n';
}

void JMGraphics::Mesh::readData(std::istream& stream, std::vector<JMGraphics::Mesh::Vertex>& verts, std::vector<unsigned int>& indices) {
	std::string dataCheck;

	std::getline(stream, dataCheck);

	if (dataCheck != "MESH_DATA") {
		std::cerr << "JMGraphics::Mesh readData Error: Stream does not start with mesh data." << std::endl;
		return;
	}

	verts.clear();
	indices.clear();

	int vertNum;
	stream >> vertNum;
	stream.ignore();

	std::vector<float> data;
	data.resize(vertNum * 12);
	stream.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(vertNum * 12 * sizeof(float)));

	verts.reserve(vertNum);
	for (int i = 0; i < vertNum; i++) {
		JMGraphics::Mesh::Vertex v;
		v.pos.x = data[i * 12 + 0];
		v.pos.y = data[i * 12 + 1];
		v.pos.z = data[i * 12 + 2];

		v.texCoord.x = data[i * 12 + 3];
		v.texCoord.y = data[i * 12 + 4];

		v.color.x = data[i * 12 + 5];
		v.color.y = data[i * 12 + 6];
		v.color.z = data[i * 12 + 7];
		v.color.w = data[i * 12 + 8];

		v.normal.x = data[i * 12 + 9];
		v.normal.y = data[i * 12 + 10];
		v.normal.z = data[i * 12 + 11];
		verts.push_back(v);
	}
	stream.ignore();

	int indexNum;
	stream >> indexNum;

	indices.reserve(indexNum);
	for (int i = 0; i < indexNum; i++) {
		unsigned int ind;
		stream >> ind;
		indices.push_back(ind);
	}

	stream.ignore();
}

void JMGraphics::Mesh::readData(std::istream& stream) {
	readData(stream, vertices, indices);
}