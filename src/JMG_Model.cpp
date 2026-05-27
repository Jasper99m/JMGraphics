#include "../JMGraphics.h"

JMGraphics::Model::Model(JMGraphics* Parent) {
	parent = Parent;
}
JMGraphics::Model::~Model() {
	clearMeshes();
}
bool JMGraphics::Model::load(std::string filePath) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "JMGraphics::Mesh::load() Error: Could not load file: " << filePath << "  Error code: " << importer.GetErrorString() << std::endl;
		return false;
	}

	directory = filePath.substr(0, filePath.find_last_of("/"));
	processNodes(scene->mRootNode, scene);
	return true;
}

void JMGraphics::Model::processNodes(aiNode* node, const aiScene* scene) {

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNodes(node->mChildren[i], scene);
	}
}

JMGraphics::Mesh* JMGraphics::Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<JMGraphics::Mesh::Vertex> vertices;
	std::vector<unsigned int> indeces;
	std::vector<JMGraphics::Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		JMGraphics::Mesh::Vertex vert;

		vert.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vert.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		if (mesh->mTextureCoords[0]) {
			vert.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else {
			vert.texCoord = glm::vec2(0.0f);
		}

		vertices.push_back(vert);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indeces.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<JMGraphics::Texture> diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<JMGraphics::Texture> specularMaps = loadTextures(material, aiTextureType_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	//std::cout << "Loading mesh with " << vertices.size() << " verts and " << indeces.size() << " indices." << std::endl;

	JMGraphics::Mesh* newMesh = new JMGraphics::Mesh(parent);
	newMesh->vertices = vertices;
	newMesh->indices = indeces;
	newMesh->textures = textures;
	newMesh->process();
	return newMesh;
}

std::vector<JMGraphics::Texture> JMGraphics::Model::loadTextures(aiMaterial* material, aiTextureType type) {

	std::vector<JMGraphics::Texture> textures;

	for (unsigned int i = 0; i < material->GetTextureCount(type); i++) 
	{
		aiString str;
		material->GetTexture(type, i, &str);

		Texture tex(directory);
		textures.push_back(tex);
		loadedTextures.push_back(tex);
	}

	return textures;
}
void JMGraphics::Model::clearMeshes() {
	for (JMGraphics::Mesh* i : meshes) {
		delete(i);
	}
	meshes.clear();
}
void JMGraphics::Model::display() {
	for (JMGraphics::Mesh* i : meshes) {
		i->display();
	}
}

JMGraphics::Model* JMGraphics::Model::clone() const{
	JMGraphics::Model* c = new JMGraphics::Model(this->parent);
	c->name = name;
	c->directory = directory;
	c->loadedTextures = loadedTextures;
	for (int i = 0; i < meshes.size(); i++) {
		c->meshes.push_back(meshes[i]->clone());
	}
	return c;
}

void JMGraphics::Model::wireframeOn() {
	for (JMGraphics::Mesh* i : meshes) {
		i->renderWireframe = true;
	}
}
void JMGraphics::Model::wireframeOff() {
	for (JMGraphics::Mesh* i : meshes) {
		i->renderWireframe = false;
	}
}
void JMGraphics::Model::facesOn() {
	for (JMGraphics::Mesh* i : meshes) {
		i->renderFaces = true;
	}
}
void JMGraphics::Model::facesOff() {
	for (JMGraphics::Mesh* i : meshes) {
		i->renderFaces = false;
	}
}
void JMGraphics::Model::setWireframeColor(glm::vec4 color) {
	for (JMGraphics::Mesh* i : meshes) {
		i->wireframeColor = color;
	}
}