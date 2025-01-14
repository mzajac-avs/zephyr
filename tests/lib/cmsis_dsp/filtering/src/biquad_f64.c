/*
 * Copyright (c) 2020 Stephanos Ioannidis <root@stephanos.io>
 * Copyright (C) 2010-2020 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <zephyr/kernel.h>
#include <stdlib.h>
#include <arm_math.h>
#include "../../common/test_common.h"

#include "biquad_f64.pat"

#define SNR_ERROR_THRESH	((float64_t)98)
#define REL_ERROR_THRESH	(1.2e-3)

static void test_arm_biquad_cascade_df2t_f64_default(void)
{
	size_t index;
	size_t length = ARRAY_SIZE(ref_default);
	size_t block_size = length / 2;
	const float64_t *input = (const float64_t *)in_default_val;
	const float64_t *coeff = (const float64_t *)in_default_coeff;
	const float64_t *ref = (const float64_t *)ref_default;
	float64_t *state, *output_buf, *output;
	arm_biquad_cascade_df2T_instance_f64 inst;

	/* Allocate buffers */
	state = malloc(128 * sizeof(float64_t));
	zassert_not_null(state, ASSERT_MSG_BUFFER_ALLOC_FAILED);

	output_buf = malloc(length * sizeof(float64_t));
	zassert_not_null(output_buf, ASSERT_MSG_BUFFER_ALLOC_FAILED);

	output = output_buf;

	/* Initialise instance */
	arm_biquad_cascade_df2T_init_f64(&inst, 3, coeff, state);

	/* TODO: Add NEON support */

	/* Enumerate blocks */
	for (index = 0; index < 2; index++) {
		/* Run test function */
		arm_biquad_cascade_df2T_f64(
			&inst, input, output, block_size);

		/* Increment pointers */
		input += block_size;
		output += block_size;
	}

	/* Validate output */
	zassert_true(
		test_snr_error_f64(length, output_buf, ref, SNR_ERROR_THRESH),
		ASSERT_MSG_SNR_LIMIT_EXCEED);

	zassert_true(
		test_rel_error_f64(length, output_buf, ref, REL_ERROR_THRESH),
		ASSERT_MSG_REL_ERROR_LIMIT_EXCEED);

	/* Free buffers */
	free(state);
	free(output_buf);
}

static void test_arm_biquad_cascade_df2t_f64_rand(void)
{
	size_t sample_index, stage_count, block_size;
	size_t sample_count = ARRAY_SIZE(in_rand_config) / 2;
	size_t length = ARRAY_SIZE(ref_rand_mono);
	const uint16_t *config = in_rand_config;
	const float64_t *input = (const float64_t *)in_rand_mono_val;
	const float64_t *coeff = (const float64_t *)in_rand_coeff;
	const float64_t *ref = (const float64_t *)ref_rand_mono;
	float64_t *state, *output_buf, *output;
	arm_biquad_cascade_df2T_instance_f64 inst;

	/* Allocate buffers */
	state = malloc(128 * sizeof(float64_t));
	zassert_not_null(state, ASSERT_MSG_BUFFER_ALLOC_FAILED);

	output_buf = malloc(length * sizeof(float64_t));
	zassert_not_null(output_buf, ASSERT_MSG_BUFFER_ALLOC_FAILED);

	output = output_buf;

	/* Enumerate samples */
	for (sample_index = 0; sample_index < sample_count; sample_index++) {
		/* Resolve sample configurations */
		stage_count = config[0];
		block_size = config[1];

		/* Initialise instance */
		arm_biquad_cascade_df2T_init_f64(
			&inst, stage_count, coeff, state);

		/* TODO: Add NEON support */

		/* Run test function */
		arm_biquad_cascade_df2T_f64(
			&inst, input, output, block_size);

		/* Increment pointers */
		input += block_size;
		output += block_size;
		coeff += stage_count * 5;
		config += 2;
	}

	/* Validate output */
	zassert_true(
		test_snr_error_f64(length, output_buf, ref, SNR_ERROR_THRESH),
		ASSERT_MSG_SNR_LIMIT_EXCEED);

	zassert_true(
		test_rel_error_f64(length, output_buf, ref, REL_ERROR_THRESH),
		ASSERT_MSG_REL_ERROR_LIMIT_EXCEED);

	/* Free buffers */
	free(state);
	free(output_buf);
}

void test_filtering_biquad_f64(void)
{
	ztest_test_suite(filtering_biquad_f64,
		ztest_unit_test(test_arm_biquad_cascade_df2t_f64_default),
		ztest_unit_test(test_arm_biquad_cascade_df2t_f64_rand)
		);

	ztest_run_test_suite(filtering_biquad_f64);
}
