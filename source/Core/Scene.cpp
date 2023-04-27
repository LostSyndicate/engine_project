//
// Created by losts_n1cs3jj on 11/5/2022.
//

#include "Core/Scene.h"

#include "Graphics/Model.h"

void Scene::ImportScene(const char* path)
{
	FILE* file = fopen(path, "r");
	if (file == NULL)
	{
		printf("Could not open file %s\n.", path);
		return;
	}

	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* data = (char*)malloc(size);
	fread(data, 1, size + 1, file);
	data[size] = 0;

	if (strncmp(data, "<!--SM3-->", 11) != 0)
	{
//		garphics->AddMesh(mesh);

		free(data);
		fclose(file);
		printf("Scene::ImportScene currently only accept SM3 scene files.");
		return;
	}
}