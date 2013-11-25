void CvSVM::write_params( CvFileStorage* fs ) const
{
    //CV_FUNCNAME( "CvSVM::write_params" );

    __BEGIN__;

    int svm_type = params.svm_type;
    int kernel_type = params.kernel_type;

    const char* svm_type_str =
        svm_type == CvSVM::C_SVC ? "C_SVC" :
        svm_type == CvSVM::NU_SVC ? "NU_SVC" :
        svm_type == CvSVM::ONE_CLASS ? "ONE_CLASS" :
        svm_type == CvSVM::EPS_SVR ? "EPS_SVR" :
        svm_type == CvSVM::NU_SVR ? "NU_SVR" : 0;
    const char* kernel_type_str =
        kernel_type == CvSVM::LINEAR ? "LINEAR" :
        kernel_type == CvSVM::POLY ? "POLY" :
        kernel_type == CvSVM::RBF ? "RBF" :
        kernel_type == CvSVM::SIGMOID ? "SIGMOID" : 0;

    if( svm_type_str )
        cvWriteString( fs, "svm_type", svm_type_str );
    else
        cvWriteInt( fs, "svm_type", svm_type );

    // save kernel
    cvStartWriteStruct( fs, "kernel", CV_NODE_MAP + CV_NODE_FLOW );

    if( kernel_type_str )
        cvWriteString( fs, "type", kernel_type_str );
    else
        cvWriteInt( fs, "type", kernel_type );

    if( kernel_type == CvSVM::POLY || !kernel_type_str )
        cvWriteReal( fs, "degree", params.degree );

    if( kernel_type != CvSVM::LINEAR || !kernel_type_str )
        cvWriteReal( fs, "gamma", params.gamma );

    if( kernel_type == CvSVM::POLY || kernel_type == CvSVM::SIGMOID || !kernel_type_str )
        cvWriteReal( fs, "coef0", params.coef0 );

    cvEndWriteStruct(fs);

    if( svm_type == CvSVM::C_SVC || svm_type == CvSVM::EPS_SVR ||
        svm_type == CvSVM::NU_SVR || !svm_type_str )
        cvWriteReal( fs, "C", params.C );

    if( svm_type == CvSVM::NU_SVC || svm_type == CvSVM::ONE_CLASS ||
        svm_type == CvSVM::NU_SVR || !svm_type_str )
        cvWriteReal( fs, "nu", params.nu );

    if( svm_type == CvSVM::EPS_SVR || !svm_type_str )
        cvWriteReal( fs, "p", params.p );

    cvStartWriteStruct( fs, "term_criteria", CV_NODE_MAP + CV_NODE_FLOW );
    if( params.term_crit.type & CV_TERMCRIT_EPS )
        cvWriteReal( fs, "epsilon", params.term_crit.epsilon );
    if( params.term_crit.type & CV_TERMCRIT_ITER )
        cvWriteInt( fs, "iterations", params.term_crit.max_iter );
    cvEndWriteStruct( fs );

    __END__;
}


void CvSVM::write( CvFileStorage* fs, const char* name ) const
{
    CV_FUNCNAME( "CvSVM::write" );

    __BEGIN__;

    int i, var_count = get_var_count(), df_count, class_count;
    const CvSVMDecisionFunc* df = decision_func;

    cvStartWriteStruct( fs, name, CV_NODE_MAP, CV_TYPE_NAME_ML_SVM );

    write_params( fs );

    cvWriteInt( fs, "var_all", var_all );
    cvWriteInt( fs, "var_count", var_count );

    class_count = class_labels ? class_labels->cols :
                  params.svm_type == CvSVM::ONE_CLASS ? 1 : 0;

    if( class_count )
    {
        cvWriteInt( fs, "class_count", class_count );

        if( class_labels )
            cvWrite( fs, "class_labels", class_labels );

        if( class_weights )
            cvWrite( fs, "class_weights", class_weights );
    }

    if( var_idx )
        cvWrite( fs, "var_idx", var_idx );

    // write the joint collection of support vectors
    cvWriteInt( fs, "sv_total", sv_total );
    cvStartWriteStruct( fs, "support_vectors", CV_NODE_SEQ );
    for( i = 0; i < sv_total; i++ )
    {
        cvStartWriteStruct( fs, 0, CV_NODE_SEQ + CV_NODE_FLOW );
        cvWriteRawData( fs, sv[i], var_count, "f" );
        cvEndWriteStruct( fs );
    }

    cvEndWriteStruct( fs );

    // write decision functions
    df_count = class_count > 1 ? class_count*(class_count-1)/2 : 1;
    df = decision_func;

    cvStartWriteStruct( fs, "decision_functions", CV_NODE_SEQ );
    for( i = 0; i < df_count; i++ )
    {
        int sv_count = df[i].sv_count;
        cvStartWriteStruct( fs, 0, CV_NODE_MAP );
        cvWriteInt( fs, "sv_count", sv_count );
        cvWriteReal( fs, "rho", df[i].rho );
        cvStartWriteStruct( fs, "alpha", CV_NODE_SEQ+CV_NODE_FLOW );
        cvWriteRawData( fs, df[i].alpha, df[i].sv_count, "d" );
        cvEndWriteStruct( fs );
        if( class_count > 1 )
        {
            cvStartWriteStruct( fs, "index", CV_NODE_SEQ+CV_NODE_FLOW );
            cvWriteRawData( fs, df[i].sv_index, df[i].sv_count, "i" );
            cvEndWriteStruct( fs );
        }
        else
            CV_ASSERT( sv_count == sv_total );
        cvEndWriteStruct( fs );
    }
    cvEndWriteStruct( fs );
    cvEndWriteStruct( fs );

    __END__;
}
