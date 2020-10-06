#include "gr_sample_sink.h"

gr_sample_sink_sptr
make_gr_sample_sink ()
{
    return gnuradio::get_initial_sptr(new gr_sample_sink);
}

gr_sample_sink::gr_sample_sink() :
        gr::sync_block("gr_sample_sink",
                       gr::io_signature::make (1, 1, sizeof (gr_complex)),
                       gr::io_signature::make (0, 0, 0))
{
    _offset = 0;
    _finished = false;
    _data = new std::vector<gr_complex>;

}

gr_sample_sink::~gr_sample_sink()
{
    _data->clear();
    delete _data;
}

void gr_sample_sink::flush()
{
    gr::thread::scoped_lock guard(_mutex);
    _data->clear();
}

std::vector<gr_complex>* gr_sample_sink::get_data()
{
    gr::thread::scoped_lock guard(_mutex);
    if(_data->size() < 64)
    {
        return nullptr;
    }
    std::vector<gr_complex>* data = new std::vector<gr_complex>;
    data->reserve(_data->size());
    data->insert(data->end(),_data->begin(),_data->end());
    _data->clear();

    return data;
}

int gr_sample_sink::work(int noutput_items,
       gr_vector_const_void_star &input_items,
       gr_vector_void_star &output_items)
{
    (void) output_items;
    if(noutput_items < 1)
    {
        return noutput_items;
    }
    gr::thread::scoped_lock guard(_mutex);
    gr_complex *in = (gr_complex*)(input_items[0]);
    if(_data->size() > 1024 * 1024 * 10)
    {
        /// not reading data fast enough, anything more than 1 sec
        /// of data in the buffer is a problem downstream so dropping buffer
        return noutput_items;
    }
    for(int i=0;i < noutput_items;i++)
    {
        _data->push_back(in[i]);
    }

    return noutput_items;
}
