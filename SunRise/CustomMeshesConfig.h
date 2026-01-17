#pragma once
#include "Config.h"

void LoadCustomMeshes()
{
	CustomMeshes.Clear();

	YAML::Node meshesRoot = YAML::LoadFile(GetConfigFolder("Meshes.yml"));

	const auto& meshes = meshesRoot["Meshes"];

	for (const auto& node : meshes)
	{
		CustomMesh mesh;

		mesh.Name = node["Solid"].as<std::string>();
		mesh.Position = ParseVec3(node["Position"]);
		mesh.Rotation = ParseVec3(node["Rotation"]);
		mesh.Scale = ParseVec3(node["Scale"]);
		mesh.Distance = YmlGet(node, "Distance", -1.0f);
		mesh.CastShadow = YmlGet(node, "CastShadow", true);
		mesh.SetMatrix();

		CustomMeshes.Add(mesh);
	}
}

void SaveCustomMeshes()
{
	YAML::Node list;
	for (auto& mesh : CustomMeshes.GetList())
	{
		YAML::Node node;

		node["Solid"] = mesh.Name;
		node["Position"] = SerializeVector3(mesh.Position);
		node["Rotation"] = SerializeVector3(mesh.Rotation);
		node["Scale"] = SerializeVector3(mesh.Scale);
		node["Distance"] = mesh.Distance;
		node["CastShadow"] = mesh.CastShadow;

		list.push_back(node);
	}

	YAML::Node root;
	root["Meshes"] = list;

	std::ofstream fout(GetConfigFolder("Meshes.yml"));
	fout << root;
	fout.close();
}