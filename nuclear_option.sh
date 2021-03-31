echo "\nError code 10\n"
./etapa4 < test_10_undeclared_a; echo $?;
./etapa4 < test_10_undeclared_b; echo $?;
./etapa4 < test_10_undeclared_c; echo $?;
./etapa4 < test_10_undeclared_d; echo $?;
./etapa4 < test_10_undeclared_e; echo $?;
./etapa4 < test_10_undeclared_f; echo $?;
echo "\nError code 11\n" 
./etapa4 < test_11_declared_a; echo $?;      
./etapa4 < test_11_declared_b; echo $?;
./etapa4 < test_11_declared_c; echo $?;
./etapa4 < test_11_declared_d; echo $?;
./etapa4 < test_11_declared_e; echo $?;
echo "\nError code 20\n"
./etapa4 < test_20_variable_a; echo $?;
./etapa4 < test_20_variable_b; echo $?;
echo "\nError code 21\n"
./etapa4 < test_21_vector_a; echo $?;
./etapa4 < test_21_vector_b; echo $?;
echo "\nError code 22\n"
./etapa4 < test_22_function_a; echo $?;
./etapa4 < test_22_function_b; echo $?;
echo "\nError code 30\n"
./etapa4 < test_30_wrong_type_a; echo $?;
./etapa4 < test_30_wrong_type_b; echo $?;
./etapa4 < test_30_wrong_type_c; echo $?;
./etapa4 < test_30_wrong_type_d; echo $?;
./etapa4 < test_30_wrong_type_e; echo $?;
./etapa4 < test_30_wrong_type_f; echo $?;
echo "\nError code 31\n"
./etapa4 < test_31_string_to_x_a; echo $?;
./etapa4 < test_31_string_to_x_b; echo $?;
echo "\nError code 32\n"
./etapa4 < test_32_char_to_x_a; echo $?;
./etapa4 < test_32_char_to_x_b; echo $?;
echo "\nError code 33\n"
./etapa4 < test_33_string_max_a; echo $?;
./etapa4 < test_33_string_max_b; echo $?;
./etapa4 < test_33_string_max_c; echo $?;
echo "\nError code 34\n"
./etapa4 < test_34_string_vector_a; echo $?;
echo "\nError code 40\n"
./etapa4 < test_40_missing_args_a; echo $?;
./etapa4 < test_40_missing_args_b; echo $?;
echo "\nError code 41\n"
./etapa4 < test_41_excess_args_a; echo $?;
./etapa4 < test_41_excess_args_b; echo $?;
echo "\nError code 42\n"
./etapa4 < test_42_wrng_type_args_a; echo $?;
./etapa4 < test_42_wrng_type_args_b; echo $?;
./etapa4 < test_42_wrng_type_args_c; echo $?;
./etapa4 < test_42_wrng_type_args_d; echo $?;
echo "\nError code 43\n"
./etapa4 < test_43_function_string_a; echo $?;
./etapa4 < test_43_function_string_b; echo $?;
./etapa4 < test_43_function_string_c; echo $?;
echo "\nError code 50\n"
./etapa4 < test_50_wrong_par_input_a; echo $?;
./etapa4 < test_50_wrong_par_input_b; echo $?;
./etapa4 < test_50_wrong_par_input_c; echo $?;
echo "\nError code 51\n"
./etapa4 < test_51_wrong_par_output_a; echo $?;
./etapa4 < test_51_wrong_par_output_b; echo $?;
./etapa4 < test_51_wrong_par_output_c; echo $?;
./etapa4 < test_51_wrong_par_output_d; echo $?;
./etapa4 < test_51_wrong_par_output_e; echo $?;
./etapa4 < test_51_wrong_par_output_f; echo $?;
echo "\nError code 52\n"
./etapa4 < test_52_wrong_par_return_a; echo $?;
./etapa4 < test_52_wrong_par_return_b; echo $?;
echo "\nError code 53\n"
./etapa4 < test_53_wrong_par_shift_a; echo $?;
./etapa4 < test_53_wrong_par_shift_b; echo $?;