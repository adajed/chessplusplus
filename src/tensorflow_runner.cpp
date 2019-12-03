#include "tensorflow_runner.h"

#include <cassert>

namespace engine
{
namespace tensorflow
{

void free_buffer(void* data, size_t length)
{
    free(data);
}

void deallocator(void* data, size_t length, void *arg)
{
}

TF_Buffer* read_file(const char* file_path)
{
    FILE *file = fopen(file_path, "rb");
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    void *data = malloc(file_size);
    fread(data, file_size, 1, file);
    fclose(file);

    TF_Buffer *buffer = TF_NewBuffer();
    buffer->data = data;
    buffer->length = file_size;
    buffer->data_deallocator = free_buffer;

    return buffer;
}

Runner::Runner()
    : status(nullptr)
    , session(nullptr)
    , graph(nullptr)
    , input_opers()
    , output_opers()
    , input_tensors()
    , output_tensors()
{
}

Runner::Runner(std::string model_path,
               std::vector<Tensor> inputs,
               std::vector<Tensor> outputs)
    : status(nullptr)
    , session(nullptr)
    , graph(nullptr)
    , input_opers()
    , output_opers()
    , input_tensors()
    , output_tensors()
{
    status = TF_NewStatus();
    TF_Buffer* graph_buffer = read_file(model_path.c_str());
    graph = TF_NewGraph();
    TF_ImportGraphDefOptions* opts = TF_NewImportGraphDefOptions();
    TF_GraphImportGraphDef(graph, graph_buffer, opts, status);
    if (TF_GetCode(status) == TF_OK)
    {
        for (const auto& tensor : inputs)
        {
            input_opers.push_back({TF_GraphOperationByName(graph, tensor.opname.c_str()), tensor.index});
            input_tensors.push_back(tensor.tensor);
            assert(input_opers.back().oper != nullptr);
        }
        for (const auto& tensor : outputs)
        {
            output_opers.push_back({TF_GraphOperationByName(graph, tensor.opname.c_str()), tensor.index});
            output_tensors.push_back(tensor.tensor);
            assert(output_opers.back().oper != nullptr);
        }

        TF_SessionOptions* sess_opts = TF_NewSessionOptions();
        session = TF_NewSession(graph, sess_opts, status);
        assert(session != nullptr);
    }
    else
        assert(false);
}

float* Runner::get_input_buffer(int index)
{
    return static_cast<float*>(TF_TensorData(input_tensors[index]));
}

float* Runner::get_output_buffer(int index)
{
    return static_cast<float*>(TF_TensorData(output_tensors[index]));
}

void Runner::run()
{
    TF_SessionRun(session, nullptr,
                  input_opers.data(), input_tensors.data(), input_opers.size(),
                  output_opers.data(), output_tensors.data(), output_opers.size(),
                  nullptr, 0, nullptr, status);
}

}
}
