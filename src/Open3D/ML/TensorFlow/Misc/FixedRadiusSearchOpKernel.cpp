// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2020 www.open3d.org
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------
//

#include "FixedRadiusSearchOpKernel.h"
#include "Open3D/ML/Misc/Detail/FixedRadiusSearch.h"

using namespace open3d::ml::detail;
using namespace fixed_radius_search_opkernel;
using namespace tensorflow;

template <class T>
class FixedRadiusSearchOpKernelCPU : public FixedRadiusSearchOpKernel {
public:
    explicit FixedRadiusSearchOpKernelCPU(OpKernelConstruction* construction)
        : FixedRadiusSearchOpKernel(construction) {}

    void Kernel(tensorflow::OpKernelContext* context,
                const tensorflow::Tensor& points,
                const tensorflow::Tensor& queries,
                const tensorflow::Tensor& radius,
                const tensorflow::Tensor& hash_table_index,
                const tensorflow::Tensor& hash_table_row_splits,
                tensorflow::Tensor& query_neighbors_row_splits) {
        OutputAllocator<T> output_allocator(context);

        FixedRadiusSearchCPU(
                (int64_t*)query_neighbors_row_splits.flat<int64>().data(),
                points.shape().dim_size(0), points.flat<T>().data(),
                queries.shape().dim_size(0), queries.flat<T>().data(),
                radius.scalar<T>()(), hash_table_row_splits.shape().dim_size(0),
                hash_table_row_splits.flat<uint32_t>().data(),
                hash_table_index.flat<uint32_t>().data(), metric,
                ignore_query_point, return_distances, output_allocator);
    }
};

#define REG_KB(type)                                            \
    REGISTER_KERNEL_BUILDER(Name("Open3DFixedRadiusSearch")     \
                                    .Device(DEVICE_CPU)         \
                                    .TypeConstraint<type>("T"), \
                            FixedRadiusSearchOpKernelCPU<type>);
REG_KB(float)
REG_KB(double)
#undef REG_KB
