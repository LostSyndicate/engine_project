#ifndef SM3_GPUTIMEQUERY_H
#define SM3_GPUTIMEQUERY_H

#include <glad/glad.h>

#include "Core/Reference.h"

class GPUQuery
{
public:
	enum Type
	{
		INVALID,
		TIME
	};

	GPUQuery(Type type);
	~GPUQuery();

	void Begin();
	void End();
	bool IsResultAvailable();
	void GetResult(void* out_data, size_t data_sizeof, size_t* out_size = nullptr);
	Type GetType() const;
private:
	Type type_{ INVALID };
	GLuint query_id_{ 0 };
};

#endif