#include <turbodbc/buffer_size.h>

namespace turbodbc {


rows::rows():
    value(1)
{
}

rows::rows(std::size_t rows_to_buffer_):
    value(rows_to_buffer_)
{
}

megabytes::megabytes(std::size_t megabytes_to_buffer) :
    value(megabytes_to_buffer)
{
}

determine_rows_to_buffer::determine_rows_to_buffer(std::vector<std::unique_ptr<description const>> const& descriptions) :
    descriptions_(descriptions)
{
}

std::size_t determine_rows_to_buffer::operator()(rows const& r) const
{
    if (r.value > 0) {
        return r.value;
    } else {
        return 1;
    }
}

std::size_t determine_rows_to_buffer::operator()(megabytes const& m) const
{
    std::size_t bytes_per_row = 0;
    for (auto & d : descriptions_) {
        bytes_per_row += d->element_size();
    }
    auto const bytes_to_buffer = m.value * 1024 * 1024;
    auto const rows_to_buffer = bytes_to_buffer / bytes_per_row;

    if (rows_to_buffer > 0) {
        return rows_to_buffer;
    } else {
        return 1;
    }
}

buffer_size halve_buffer_size::operator()(rows const& r) const
{
    return {rows((r.value + 1) / 2)};
}

buffer_size halve_buffer_size::operator()(megabytes const& m) const
{
    return {megabytes((m.value + 1) / 2)};
}

}