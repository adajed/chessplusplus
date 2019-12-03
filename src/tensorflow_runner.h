#ifndef CHESS_ENGINE_TENSORFLOW_RUNNER_H_
#define CHESS_ENGINE_TENSORFLOW_RUNNER_H_

#include <string>
#include <vector>

#include <tensorflow/c/c_api.h>

namespace engine
{
namespace tensorflow
{

using Shape = std::vector<int64_t>;

struct Tensor
{
    std::string opname;
    int index;
    TF_Tensor* tensor;
};

class Runner
{
    public:
        Runner();
        Runner(std::string model_path,
               std::vector<Tensor> inputs,
               std::vector<Tensor> outputs);

        void run();

        float* get_input_buffer(int index);

        float* get_output_buffer(int index);

    private:
        TF_Status* status;
        TF_Session* session;
        TF_Graph* graph;

        std::vector<TF_Output> input_opers;
        std::vector<TF_Output> output_opers;
        std::vector<TF_Tensor*> input_tensors;
        std::vector<TF_Tensor*> output_tensors;
};


}  // namespace tensorflow
}  // namespace engine

#endif  // CHESS_ENGINE_TENSORFLOW_RUNNER_H_
