/*
 * Copyright (C) 2018 GreenWaves Technologies
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef __FIR_GENERATOR_H__
#define __FIR_GENERATOR_H__

/** @addtogroup groupFIR
@{ */

/** @defgroup FIRGene FIRGenerator
@{ */

/**
@brief Load FIR basic kernels

Load FIR basic kernels
*/
void LoadFIRLibrary();


/**
@brief Generate FIR for a given set of parameters for Q15 input and output data

Generate FIR for a given set of parameters

        Based on actual number of Coefficients for the FIR filter we select which basic kernel to be used, for
small number of coefficients we have specially optimized basic kernels. To force generic the usage of generic
implementations set Generic argument to 1. If Generic is selected you can also use Scalar to force the usage
of a scalar implementation instead of a vectorial one.

        Inputs and outputs are double buffered to make sur that the computing kernels
are always kept buzzy. Input and output are objects of width 1 and height NSamples, tiling is performed horizontally.

        We need a delay line capability for the input, this delay line should be able to store NCoeffs value. The
basic kernel assumes that this delay line should be placed right before the input, alltogether a single chunck of
memory of size NSample+NCoeffs of Word16. To achieve this we use the KerArgPad() construct instead of the usual
KerArg() one where extra fixed size padding before or after the user kernel argument can be delcared, in this case
we need extra padding before the input hence the NCoeefs, 0 part.

        The second parameter of the FIR kernel, NextIn, should point to the next double buffer, this is why we use
the KER_ARG_TILE_NEXTDB property in the call sequence.
*/
void GeneratorFIR(
	char *Name,			/**< C Name of the function AutoTiler will generate */
	unsigned int NSamples,		/**< Number of input samples */
	unsigned int NCoeffs,		/**< Number of Taps (fir filter coefficients) */
	unsigned int Generic,		/**< 1: Use generic filter, 0: if available use a specialized version */
	unsigned int Scalar		/**< 1: Use a scalar implementation, 0: use a vectorial implementation */
	);

void GeneratorFIR_fp16(char *Name, unsigned int NSamples, unsigned int NCoeffs);

void GeneratorFIR_fp32(char *Name, unsigned int NSamples, unsigned int NCoeffs);


/** @} */ // End of FIRGene
/** @} */

#endif


