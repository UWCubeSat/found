var hierarchy =
[
    [ "found::_d_u_", "unionfound_1_1__d__u__.html", null ],
    [ "found::_f_u_", "unionfound_1_1__f__u__.html", null ],
    [ "found::Action", "classfound_1_1Action.html", [
      [ "found::Stage< const raw_type< std::pair< EulerAngles, EulerAngles > > &, raw_type< Quaternion > >", "classfound_1_1Stage.html", null ],
      [ "found::Stage< const raw_type< Points > &, raw_type< PositionVector > >", "classfound_1_1Stage.html", null ],
      [ "found::Stage< const raw_type< Image > &, raw_type< Points > >", "classfound_1_1Stage.html", null ],
      [ "found::Stage< const raw_type< Input > &, raw_type< Output > >", "classfound_1_1Stage.html", [
        [ "found::FunctionStage< Input, Output >", "classfound_1_1FunctionStage.html", [
          [ "found::Pipeline< T, T, DEFAULT_NUM_STAGES >", "classfound_1_1Pipeline.html", null ],
          [ "found::Pipeline< Input, Output, DEFAULT_NUM_STAGES >", "classfound_1_1Pipeline.html", null ],
          [ "found::Pipeline< std::pair< EulerAngles, EulerAngles >, Quaternion, N >", "classfound_1_1Pipeline.html", null ],
          [ "found::Pipeline< Image, PositionVector, N >", "classfound_1_1Pipeline.html", null ],
          [ "found::Pipeline< LocationRecords, LocationRecords, N >", "classfound_1_1Pipeline.html", null ],
          [ "found::Pipeline< Input, Output, N >", "classfound_1_1Pipeline.html", [
            [ "found::SequentialPipeline< std::pair< EulerAngles, EulerAngles >, Quaternion, calibration_size >", "classfound_1_1SequentialPipeline.html", null ],
            [ "found::SequentialPipeline< Image, PositionVector, distance_size >", "classfound_1_1SequentialPipeline.html", null ],
            [ "found::SequentialPipeline< LocationRecords, LocationRecords, orbit_size >", "classfound_1_1SequentialPipeline.html", null ],
            [ "found::ModifyingPipeline< T, N >", "classfound_1_1ModifyingPipeline.html", null ],
            [ "found::SequentialPipeline< Input, Output, N >", "classfound_1_1SequentialPipeline.html", null ]
          ] ]
        ] ]
      ] ],
      [ "found::Stage< const raw_type< T > &, raw_type< T > >", "classfound_1_1Stage.html", null ],
      [ "found::Stage< raw_type< T > &, void >", "classfound_1_1Stage.html", [
        [ "found::ModifyingStage< T >", "classfound_1_1ModifyingStage.html", null ]
      ] ],
      [ "found::Stage< const raw_type< LocationRecords > &, raw_type< LocationRecords > >", "classfound_1_1Stage.html", null ],
      [ "found::Stage< const raw_type< PositionVector > &, raw_type< PositionVector > >", "classfound_1_1Stage.html", null ],
      [ "found::Stage< const raw_type< Image > &, raw_type< PositionVector > >", "classfound_1_1Stage.html", null ],
      [ "found::Stage< Input, Output >", "classfound_1_1Stage.html", [
        [ "found::FunctionStage< std::pair< EulerAngles, EulerAngles >, Quaternion >", "classfound_1_1FunctionStage.html", [
          [ "found::CalibrationAlgorithm", "classfound_1_1CalibrationAlgorithm.html", [
            [ "found::LOSTCalibrationAlgorithm", "classfound_1_1LOSTCalibrationAlgorithm.html", null ]
          ] ]
        ] ],
        [ "found::FunctionStage< Points, PositionVector >", "classfound_1_1FunctionStage.html", [
          [ "found::DistanceDeterminationAlgorithm", "classfound_1_1DistanceDeterminationAlgorithm.html", [
            [ "found::EllipticDistanceDeterminationAlgorithm", "classfound_1_1EllipticDistanceDeterminationAlgorithm.html", null ],
            [ "found::SphericalDistanceDeterminationAlgorithm", "classfound_1_1SphericalDistanceDeterminationAlgorithm.html", [
              [ "found::IterativeSphericalDistanceDeterminationAlgorithm", "classfound_1_1IterativeSphericalDistanceDeterminationAlgorithm.html", null ]
            ] ]
          ] ]
        ] ],
        [ "found::FunctionStage< Image, Points >", "classfound_1_1FunctionStage.html", [
          [ "found::EdgeDetectionAlgorithm", "classfound_1_1EdgeDetectionAlgorithm.html", [
            [ "found::LoCEdgeDetectionAlgorithm", "classfound_1_1LoCEdgeDetectionAlgorithm.html", null ],
            [ "found::SimpleEdgeDetectionAlgorithm", "classfound_1_1SimpleEdgeDetectionAlgorithm.html", null ]
          ] ]
        ] ],
        [ "found::FunctionStage< T, T >", "classfound_1_1FunctionStage.html", null ],
        [ "found::FunctionStage< LocationRecords, LocationRecords >", "classfound_1_1FunctionStage.html", [
          [ "found::OrbitPropagationAlgorithm", "classfound_1_1OrbitPropagationAlgorithm.html", [
            [ "found::ApproximateOrbitPropagationAlgorithm", "classfound_1_1ApproximateOrbitPropagationAlgorithm.html", null ]
          ] ]
        ] ],
        [ "found::FunctionStage< PositionVector, PositionVector >", "classfound_1_1FunctionStage.html", [
          [ "found::VectorGenerationAlgorithm", "classfound_1_1VectorGenerationAlgorithm.html", [
            [ "found::FeatureDetectionVectorGenerationAlgorithm", "classfound_1_1FeatureDetectionVectorGenerationAlgorithm.html", null ],
            [ "found::LOSTVectorGenerationAlgorithm", "classfound_1_1LOSTVectorGenerationAlgorithm.html", null ]
          ] ]
        ] ],
        [ "found::FunctionStage< Image, PositionVector >", "classfound_1_1FunctionStage.html", null ]
      ] ]
    ] ],
    [ "found::Attitude", "classfound_1_1Attitude.html", null ],
    [ "CalibrationOptions", "classCalibrationOptions.html", null ],
    [ "found::Camera", "classfound_1_1Camera.html", null ],
    [ "found::Component", "structfound_1_1Component.html", null ],
    [ "found::DataFile", "structfound_1_1DataFile.html", null ],
    [ "found::DataFileHeader", "structfound_1_1DataFileHeader.html", null ],
    [ "found::DateTime", "structfound_1_1DateTime.html", null ],
    [ "DistanceOptions", "classDistanceOptions.html", null ],
    [ "found::EarthSphericalVec3", "structfound_1_1EarthSphericalVec3.html", null ],
    [ "found::Edge", "structfound_1_1Edge.html", null ],
    [ "found::EulerAngles", "classfound_1_1EulerAngles.html", null ],
    [ "found::Image", "structfound_1_1Image.html", null ],
    [ "found::LocationRecord", "structfound_1_1LocationRecord.html", null ],
    [ "found::Mat3", "classfound_1_1Mat3.html", null ],
    [ "OrbitOptions", "classOrbitOptions.html", null ],
    [ "found::OrbitParams", "structfound_1_1OrbitParams.html", null ],
    [ "found::PipelineExecutor", "classfound_1_1PipelineExecutor.html", [
      [ "found::CalibrationPipelineExecutor", "classfound_1_1CalibrationPipelineExecutor.html", null ],
      [ "found::DistancePipelineExecutor", "classfound_1_1DistancePipelineExecutor.html", null ],
      [ "found::OrbitPipelineExecutor", "classfound_1_1OrbitPipelineExecutor.html", null ]
    ] ],
    [ "found::Quaternion", "classfound_1_1Quaternion.html", null ],
    [ "found::Vec2", "structfound_1_1Vec2.html", null ],
    [ "found::Vec3", "classfound_1_1Vec3.html", null ]
];