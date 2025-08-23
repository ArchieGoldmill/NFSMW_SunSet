#pragma once
#include "eModel.h"

struct CustomMesh
{
	std::string Name;

	D3DXVECTOR3 Position = { 0, 0, 0 };
	D3DXVECTOR3 Rotation = { 0, 0, 0 };
	D3DXVECTOR3 Scale = { 1, 1, 1 };

	D3DXMATRIX Matrix;

	eModel Model;

	bool Selected = false;

	void SetMatrix()
	{
		D3DXMATRIX scale;
		D3DXMatrixScaling(&scale, Scale.x, Scale.y, Scale.z);

		D3DXMATRIX rot;
		D3DXMatrixRotationYawPitchRoll(&rot, D3DXToRadian(Rotation.x), D3DXToRadian(Rotation.y), D3DXToRadian(Rotation.z));

		D3DXMATRIX pos;
		D3DXMatrixTranslation(&pos, Position.x, Position.y, Position.z);

		Matrix = scale * rot * pos;
	}
};

std::vector<CustomMesh> CustomMeshes;

void LoadCustomMeshes()
{
	CustomMeshes.clear();

	YAML::Node meshesRoot = YAML::LoadFile(GetConfigFolder("Meshes.yml"));

	const auto& meshes = meshesRoot["Meshes"];

	for (const auto& node : meshes)
	{
		CustomMesh mesh;

		mesh.Name = node["Solid"].as<std::string>();
		mesh.Position = ParseVec3(node["Position"]);
		mesh.Rotation = ParseVec3(node["Rotation"]);
		mesh.Scale = ParseVec3(node["Scale"]);
		mesh.SetMatrix();
		mesh.Model.NameHash = Game::bStringHash(mesh.Name.c_str());

		CustomMeshes.push_back(mesh);
	}
}

void SaveCustomMeshes()
{
	YAML::Node list;
	for (auto& mesh : CustomMeshes)
	{
		YAML::Node node;

		node["Solid"] = mesh.Name;
		node["Position"] = SerializeVector3(mesh.Position);
		node["Rotation"] = SerializeVector3(mesh.Rotation);
		node["Scale"] = SerializeVector3(mesh.Scale);

		list.push_back(node);
	}

	YAML::Node root;
	root["Meshes"] = list;

	std::ofstream fout(GetConfigFolder("Meshes.yml"));
	fout << root;
	fout.close();
}