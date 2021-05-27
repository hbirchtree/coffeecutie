#pragma once

#include "enums/AttribMask.h"
#include "enums/ClearBufferMask.h"
#include "enums/BufferStorageMask.h"
#include "enums/ClientAttribMask.h"
#include "enums/ContextFlagMask.h"
#include "enums/ContextProfileMask.h"
#include "enums/MapBufferAccessMask.h"
#include "enums/MemoryBarrierMask.h"
#include "enums/OcclusionQueryEventMaskAMD.h"
#include "enums/SyncObjectMask.h"
#include "enums/UseProgramStageMask.h"
#include "enums/SubgroupSupportedFeatures.h"
#include "enums/TextureStorageMaskAMD.h"
#include "enums/FragmentShaderDestMaskATI.h"
#include "enums/FragmentShaderDestModMaskATI.h"
#include "enums/FragmentShaderColorModMaskATI.h"
#include "enums/TraceMaskMESA.h"
#include "enums/PathFontStyle.h"
#include "enums/PathMetricMask.h"
#include "enums/PerformanceQueryCapsMaskINTEL.h"
#include "enums/VertexHintsMaskPGI.h"
#include "enums/BufferBitQCOM.h"
#include "enums/FoveationConfigBitQCOM.h"
#include "enums/FfdMaskSGIX.h"
#include "enums/CommandOpcodesNV.h"
#include "enums/MapTextureFormatINTEL.h"
#include "enums/PathCoordType.h"
#include "enums/TransformFeedbackTokenNV.h"
#include "enums/TriangleListSUN.h"
#include "enums/Boolean.h"
#include "enums/VertexShaderWriteMaskEXT.h"
#include "enums/ClampColorModeARB.h"
#include "enums/GraphicsResetStatus.h"
#include "enums/ErrorCode.h"
#include "enums/TextureSwizzle.h"
#include "enums/StencilOp.h"
#include "enums/BlendingFactor.h"
#include "enums/SyncBehaviorFlags.h"
#include "enums/TextureCompareMode.h"
#include "enums/PathColorFormat.h"
#include "enums/CombinerBiasNV.h"
#include "enums/CombinerScaleNV.h"
#include "enums/DrawBufferMode.h"
#include "enums/PixelTexGenMode.h"
#include "enums/ReadBufferMode.h"
#include "enums/ColorBuffer.h"
#include "enums/PathGenMode.h"
#include "enums/PathTransformType.h"
#include "enums/PrimitiveType.h"
#include "enums/AccumOp.h"
#include "enums/TextureEnvMode.h"
#include "enums/LightEnvModeSGIX.h"
#include "enums/StencilFunction.h"
#include "enums/IndexFunctionEXT.h"
#include "enums/AlphaFunction.h"
#include "enums/DepthFunction.h"
#include "enums/ColorMaterialFace.h"
#include "enums/CullFaceMode.h"
#include "enums/StencilFaceDirection.h"
#include "enums/MaterialFace.h"
#include "enums/FeedbackType.h"
#include "enums/FeedBackToken.h"
#include "enums/FogMode.h"
#include "enums/FrontFaceDirection.h"
#include "enums/MapQuery.h"
#include "enums/GetMapQuery.h"
#include "enums/GetPName.h"
#include "enums/VertexShaderTextureUnitParameter.h"
#include "enums/EnableCap.h"
#include "enums/LightModelParameter.h"
#include "enums/FogPName.h"
#include "enums/FogParameter.h"
#include "enums/GetFramebufferParameter.h"
#include "enums/HintTarget.h"
#include "enums/PixelMap.h"
#include "enums/GetPixelMap.h"
#include "enums/PixelStoreParameter.h"
#include "enums/PixelTransferParameter.h"
#include "enums/IndexMaterialParameterEXT.h"
#include "enums/MapTarget.h"
#include "enums/CopyImageSubDataTarget.h"
#include "enums/TextureTarget.h"
#include "enums/GetPointervPName.h"
#include "enums/TextureParameterName.h"
#include "enums/GetTextureParameter.h"
#include "enums/SamplerParameterF.h"
#include "enums/DebugSeverity.h"
#include "enums/HintMode.h"
#include "enums/DebugSource.h"
#include "enums/DebugType.h"
#include "enums/MaterialParameter.h"
#include "enums/FragmentLightParameterSGIX.h"
#include "enums/ColorMaterialParameter.h"
#include "enums/LightParameter.h"
#include "enums/ListMode.h"
#include "enums/VertexAttribIType.h"
#include "enums/WeightPointerTypeARB.h"
#include "enums/TangentPointerTypeEXT.h"
#include "enums/BinormalPointerTypeEXT.h"
#include "enums/ColorPointerType.h"
#include "enums/ListNameType.h"
#include "enums/NormalPointerType.h"
#include "enums/PixelType.h"
#include "enums/VertexAttribType.h"
#include "enums/VertexAttribPointerType.h"
#include "enums/ScalarType.h"
#include "enums/ReplacementCodeTypeSUN.h"
#include "enums/ElementPointerTypeATI.h"
#include "enums/MatrixIndexPointerTypeARB.h"
#include "enums/DrawElementsType.h"
#include "enums/SecondaryColorPointerTypeIBM.h"
#include "enums/IndexPointerType.h"
#include "enums/TexCoordPointerType.h"
#include "enums/VertexPointerType.h"
#include "enums/PixelFormat.h"
#include "enums/AttributeType.h"
#include "enums/UniformType.h"
#include "enums/GlslTypeToken.h"
#include "enums/MapTypeNV.h"
#include "enums/VertexWeightPointerTypeEXT.h"
#include "enums/FogCoordinatePointerType.h"
#include "enums/FogPointerTypeEXT.h"
#include "enums/FogPointerTypeIBM.h"
#include "enums/VertexAttribLType.h"
#include "enums/LogicOp.h"
#include "enums/PathFillMode.h"
#include "enums/MatrixMode.h"
#include "enums/ObjectIdentifier.h"
#include "enums/Buffer.h"
#include "enums/PixelCopyType.h"
#include "enums/InvalidateFramebufferAttachment.h"
#include "enums/InternalFormat.h"
#include "enums/CombinerComponentUsageNV.h"
#include "enums/CombinerPortionNV.h"
#include "enums/PolygonMode.h"
#include "enums/MeshMode1.h"
#include "enums/MeshMode2.h"
#include "enums/EvalMapsModeNV.h"
#include "enums/RenderingMode.h"
#include "enums/ShadingModel.h"
#include "enums/StringName.h"
#include "enums/TextureCoordName.h"
#include "enums/TextureEnvParameter.h"
#include "enums/TextureEnvTarget.h"
#include "enums/TextureGenMode.h"
#include "enums/TextureGenParameter.h"
#include "enums/BlitFramebufferFilter.h"
#include "enums/TextureMagFilter.h"
#include "enums/TextureMinFilter.h"
#include "enums/TextureWrapMode.h"
#include "enums/SamplerParameterI.h"
#include "enums/SizedInternalFormat.h"
#include "enums/InterleavedArrayFormat.h"
#include "enums/ClipPlaneName.h"
#include "enums/LightName.h"
#include "enums/BlendEquationModeEXT.h"
#include "enums/ConvolutionTarget.h"
#include "enums/ConvolutionTargetEXT.h"
#include "enums/SeparableTarget.h"
#include "enums/SeparableTargetEXT.h"
#include "enums/GetConvolutionParameter.h"
#include "enums/ConvolutionParameterEXT.h"
#include "enums/ConvolutionBorderModeEXT.h"
#include "enums/HistogramTarget.h"
#include "enums/HistogramTargetEXT.h"
#include "enums/GetHistogramParameterPNameEXT.h"
#include "enums/MinmaxTarget.h"
#include "enums/MinmaxTargetEXT.h"
#include "enums/GetMinmaxParameterPNameEXT.h"
#include "enums/SamplePatternSGIS.h"
#include "enums/SamplePatternEXT.h"
#include "enums/InternalFormatPName.h"
#include "enums/ColorTableTargetSGI.h"
#include "enums/ColorTableTarget.h"
#include "enums/GetColorTableParameterPNameSGI.h"
#include "enums/ColorTableParameterPNameSGI.h"
#include "enums/GetColorTableParameterPName.h"
#include "enums/ColorTableParameterPName.h"
#include "enums/BufferTargetARB.h"
#include "enums/PointParameterNameSGIS.h"
#include "enums/PointParameterNameARB.h"
#include "enums/TextureFilterSGIS.h"
#include "enums/TextureFilterFuncSGIS.h"
#include "enums/SpriteParameterNameSGIX.h"
#include "enums/ImageTransformPNameHP.h"
#include "enums/ImageTransformTargetHP.h"
#include "enums/ListParameterName.h"
#include "enums/PixelTexGenModeSGIX.h"
#include "enums/FfdTargetSGIX.h"
#include "enums/CullParameterEXT.h"
#include "enums/LightModelColorControl.h"
#include "enums/ProgramTarget.h"
#include "enums/FramebufferAttachmentParameterName.h"
#include "enums/FramebufferStatus.h"
#include "enums/VertexBufferObjectParameter.h"
#include "enums/BufferPNameARB.h"
#include "enums/ProgramParameterPName.h"
#include "enums/PipelineParameterName.h"
#include "enums/ProgramPropertyARB.h"
#include "enums/VertexAttribPropertyARB.h"
#include "enums/VertexArrayPName.h"
#include "enums/QueryObjectParameterName.h"
#include "enums/QueryTarget.h"
#include "enums/PixelTransformTargetEXT.h"
#include "enums/PixelTransformPNameEXT.h"
#include "enums/LightTextureModeEXT.h"
#include "enums/LightTexturePNameEXT.h"
#include "enums/PixelTexGenParameterNameSGIS.h"
#include "enums/LightEnvParameterSGIX.h"
#include "enums/FragmentLightModelParameterSGIX.h"
#include "enums/FragmentLightNameSGIX.h"
#include "enums/PixelStoreResampleMode.h"
#include "enums/TextureUnit.h"
#include "enums/CombinerRegisterNV.h"
#include "enums/UniformBlockPName.h"
#include "enums/FenceConditionNV.h"
#include "enums/FenceParameterNameNV.h"
#include "enums/CombinerVariableNV.h"
#include "enums/PathColor.h"
#include "enums/CombinerMappingNV.h"
#include "enums/CombinerParameterNV.h"
#include "enums/CombinerStageNV.h"
#include "enums/PixelStoreSubsampleRate.h"
#include "enums/TextureNormalModeEXT.h"
#include "enums/VertexArrayPNameAPPLE.h"
#include "enums/VertexAttribEnum.h"
#include "enums/ProgramStringProperty.h"
#include "enums/VertexAttribEnumNV.h"
#include "enums/VertexAttribPointerPropertyARB.h"
#include "enums/EvalTargetNV.h"
#include "enums/MapParameterNV.h"
#include "enums/MapAttribParameterNV.h"
#include "enums/ArrayObjectUsageATI.h"
#include "enums/PreserveModeATI.h"
#include "enums/ArrayObjectPNameATI.h"
#include "enums/VertexStreamATI.h"
#include "enums/GetTexBumpParameterATI.h"
#include "enums/TexBumpParameterATI.h"
#include "enums/VertexShaderOpEXT.h"
#include "enums/DataTypeEXT.h"
#include "enums/VertexShaderStorageTypeEXT.h"
#include "enums/VertexShaderCoordOutEXT.h"
#include "enums/ParameterRangeEXT.h"
#include "enums/VertexShaderParameterEXT.h"
#include "enums/GetVariantValueEXT.h"
#include "enums/VariantCapEXT.h"
#include "enums/PNTrianglesPNameATI.h"
#include "enums/QueryParameterName.h"
#include "enums/OcclusionQueryParameterNameNV.h"
#include "enums/ProgramFormat.h"
#include "enums/PixelDataRangeTargetNV.h"
#include "enums/CopyBufferSubDataTarget.h"
#include "enums/BufferStorageTarget.h"
#include "enums/BufferAccessARB.h"
#include "enums/BufferPointerNameARB.h"
#include "enums/VertexBufferObjectUsage.h"
#include "enums/BufferUsageARB.h"
#include "enums/ClampColorTargetARB.h"
#include "enums/FragmentOpATI.h"
#include "enums/SwizzleOpATI.h"
#include "enums/ObjectTypeAPPLE.h"
#include "enums/UniformPName.h"
#include "enums/SubroutineParameterName.h"
#include "enums/ShaderType.h"
#include "enums/ShaderParameterName.h"
#include "enums/ShaderBinaryFormat.h"
#include "enums/TransformFeedbackPName.h"
#include "enums/TransformFeedbackBufferMode.h"
#include "enums/ProgramInterface.h"
#include "enums/ClipControlOrigin.h"
#include "enums/CheckFramebufferStatusTarget.h"
#include "enums/FramebufferTarget.h"
#include "enums/RenderbufferParameterName.h"
#include "enums/FramebufferAttachment.h"
#include "enums/DrawBufferModeATI.h"
#include "enums/RenderbufferTarget.h"
#include "enums/ProgramStagePName.h"
#include "enums/PrecisionType.h"
#include "enums/ConditionalRenderMode.h"
#include "enums/BindTransformFeedbackTarget.h"
#include "enums/QueryCounterTarget.h"
#include "enums/ProgramResourceProperty.h"
#include "enums/VertexProvokingMode.h"
#include "enums/GetMultisamplePNameNV.h"
#include "enums/PatchParameterName.h"
#include "enums/PathStringFormat.h"
#include "enums/PathFontTarget.h"
#include "enums/PathParameter.h"
#include "enums/PathCoverMode.h"
#include "enums/PathElementType.h"
#include "enums/PathHandleMissingGlyphs.h"
#include "enums/PathListMode.h"
#include "enums/AtomicCounterBufferPName.h"
#include "enums/SyncParameterName.h"
#include "enums/SyncCondition.h"
#include "enums/SyncStatus.h"
#include "enums/ProgramInterfacePName.h"
#include "enums/FramebufferParameterName.h"
#include "enums/ClipControlDepth.h"
#include "enums/TextureLayout.h"
#include "enums/MemoryObjectParameterName.h"
#include "enums/ExternalHandleType.h"
#include "enums/SemaphoreParameterName.h"
#include "enums/FramebufferFetchNoncoherent.h"
#include "enums/ShadingRateQCOM.h"
#include "enums/HintTargetPGI.h"

} // gl::groups
