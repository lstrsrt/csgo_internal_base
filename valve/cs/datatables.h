#pragma once

namespace cs {

enum class send_prop_type {
    dpt_int,
    dpt_float,
    dpt_vector,
    dpt_vector_xy,
    dpt_string,
    dpt_array,
    dpt_datatable,
    dpt_int64
};

struct d_variant {
    union {
        float flt;
        long integer;
        char* string;
        void* data_tables;
        vec3 vector;
        int64_t int64;
    };
};

struct recv_prop;

struct recv_proxy_data {
    const recv_prop* recv_prop{ };
    d_variant value{ };
    int element{ };
    int object_id{ };
};

using recv_proxy_fn = std::add_pointer_t<void(recv_proxy_data*, void*, void*)>;

struct recv_table {
    recv_prop* props{ };
    int prop_amt{ };
    void* decoder{ };
    char* net_table_name{ };
    bool is_initialized{ };
    bool is_in_main_list{ };
};

struct recv_prop {
    char* var_name{ };
    send_prop_type recv_type{ };
    int flags{ };
    int string_buffer_size{ };
    bool is_inside_array{ };
    const void* extra_data{ };
    recv_prop* array_prop{ };
    void* array_length_proxy{ };
    recv_proxy_fn proxy_fn{ };
    void* data_table_proxy_fn{ };
    recv_table* data_table{ };
    int offset{ };
    int element_stride{ };
    int elements{ };
    const char* parent_array_prop_name{ };
};

enum class offset_type {
	normal,
	packed
};

enum class field_type {
	_void,
	_float,
	string,
	vector,
	quaternion,
	integer,
	boolean,
	_short,
	character,
	color32,
	embedded,
	custom,
	classptr,
	ehandle,
	edict,
	position_vector,
	time,
	tick,
	modelname,
	soundname,
	input,
	function,
	vmatrix,
	vmatrix_worldspace,
	matrix3x4_worldspace,
	interval,
	modelindex,
	materialindex,
	vector2d
};

struct datamap;
struct input_data;

using input_fn = void(*)(input_data&);

struct type_description {
	field_type type{ };
	const char* field_name{ };
	int field_offset{ };
	uint16_t field_size{ };
	uint16_t flags{ };
	const char* external_name{ };
	uintptr_t* save_restore_ops{ };
	input_fn input_fn{ };
	datamap* data_map{ };
	uint32_t field_size_in_bytes{ };
	type_description* override_field{ };
	uint32_t override_count{ };
	float field_tolerance{ };
	int flat_offset[2]{ };
	uint16_t flat_group{ };
};

struct datamap {
	type_description* data_description{ };
	int data_fields_count{ };
	char const* data_class_name{ };
	datamap* base_map{ };

	bool chains_validated{ };
	bool packed_offsets_computed{ };
	int packed_size{ };
};

}
