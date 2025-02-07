//
//    rfnoc-hls-neuralnet: Vivado HLS code for neural-net building blocks
//
//    Copyright (C) 2017 EJ Kreinar
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <iostream>

#include "myproject.h"
#include "parameters.h"

void myproject(
    input_t Input_layer[N_INPUT_1_1],
    result_t layer8_out[N_LAYER_6],
    unsigned short &const_size_in_1,
    unsigned short &const_size_out_1
) {

    //hls-fpga-machine-learning insert IO
    #pragma HLS ARRAY_RESHAPE variable=Input_layer complete dim=0
    #pragma HLS ARRAY_PARTITION variable=layer8_out complete dim=0
    #pragma HLS INTERFACE ap_vld port=Input_layer,layer8_out 
    #pragma HLS DATAFLOW 

    const_size_in_1 = N_INPUT_1_1;
    const_size_out_1 = N_LAYER_6;

#ifndef __SYNTHESIS__
    static bool loaded_weights = false;
    if (!loaded_weights) {
        //hls-fpga-machine-learning insert load weights
        nnet::load_weights_from_txt<hidden_layer_2_weight_t, 640>(w2, "w2.txt");
        nnet::load_weights_from_txt<hidden_layer_2_bias_t, 10>(b2, "b2.txt");
        nnet::load_weights_from_txt<hidden_layer_3_weight_t, 50>(w4, "w4.txt");
        nnet::load_weights_from_txt<hidden_layer_3_bias_t, 5>(b4, "b4.txt");
        nnet::load_weights_from_txt<output_layer_weight_t, 10>(w6, "w6.txt");
        nnet::load_weights_from_txt<output_layer_bias_t, 2>(b6, "b6.txt");
        nnet::load_weights_from_txt<batch_normalization_1_scale_t, 2>(s8, "s8.txt");
        nnet::load_weights_from_txt<batch_normalization_1_bias_t, 2>(b8, "b8.txt");
        loaded_weights = true;
    }
#endif

    // ****************************************
    // NETWORK INSTANTIATION
    // ****************************************

    //hls-fpga-machine-learning insert layers

    layer2_t layer2_out[N_LAYER_2];
    #pragma HLS ARRAY_PARTITION variable=layer2_out complete dim=0
    nnet::dense<input_t, layer2_t, config2>(Input_layer, layer2_out, w2, b2); // Hidden_layer_2

    layer3_t layer3_out[N_LAYER_2];
    #pragma HLS ARRAY_PARTITION variable=layer3_out complete dim=0
    nnet::relu<layer2_t, layer3_t, relu_config3>(layer2_out, layer3_out); // Hidden_layer_2_relu

    layer4_t layer4_out[N_LAYER_4];
    #pragma HLS ARRAY_PARTITION variable=layer4_out complete dim=0
    nnet::dense<layer3_t, layer4_t, config4>(layer3_out, layer4_out, w4, b4); // Hidden_layer_3

    layer5_t layer5_out[N_LAYER_4];
    #pragma HLS ARRAY_PARTITION variable=layer5_out complete dim=0
    nnet::relu<layer4_t, layer5_t, relu_config5>(layer4_out, layer5_out); // Hidden_layer_3_relu

    layer6_t layer6_out[N_LAYER_6];
    #pragma HLS ARRAY_PARTITION variable=layer6_out complete dim=0
    nnet::dense<layer5_t, layer6_t, config6>(layer5_out, layer6_out, w6, b6); // Output_layer

    layer7_t layer7_out[N_LAYER_6];
    #pragma HLS ARRAY_PARTITION variable=layer7_out complete dim=0
    nnet::softmax<layer6_t, layer7_t, softmax_config7>(layer6_out, layer7_out); // Output_layer_softmax

    nnet::normalize<layer7_t, result_t, config8>(layer7_out, layer8_out, s8, b8); // batch_normalization_1

}
