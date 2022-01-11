#include "stdafx.h"

#include "XRHash.h"
#include "XRPipeline.h"

#pragma region XRResourceLayout

UniformBindingInfo const* XRResourceLayout::bindingUniform(std::string uniformName, XRView<XRBuffer>* bufferView)
{
	auto found = _currentResourceBinder->_uniformList.find(uniformName);
	if (found == _currentResourceBinder->_uniformList.end())
		return nullptr;

	found->second->_bufferView = bufferView;
	return found->second;
}

UniformBindingInfo const* XRResourceLayout::bindingUniform(std::string uniformName, XRView<XRTexture>* textureView)
{
	auto found = _currentResourceBinder->_uniformList.find(uniformName);
	if (found == _currentResourceBinder->_uniformList.end())
		return nullptr;

	found->second->_textureView = textureView;
	return found->second;
}

StorageBindingInfo const* XRResourceLayout::bindingStorage(std::string storageName, XRView<XRBuffer>* bufferView)
{
	auto found = _currentResourceBinder->_storageList.find(storageName);
	if (found == _currentResourceBinder->_storageList.end())
		return nullptr;

	found->second->_bufferView = bufferView;
	return found->second;
}

StorageBindingInfo const* XRResourceLayout::bindingStorage(std::string storageName, XRView<XRTexture>* textureView)
{
	auto found = _currentResourceBinder->_storageList.find(storageName);
	if (found == _currentResourceBinder->_storageList.end())
		return nullptr;

	found->second->_textureView = textureView;
	return found->second;
}

#pragma endregion XRResourceLayout

#pragma region XRPipelineGroup

void XRPipelineGroup::AddPipelineWithPermutation(XRPipeline* pipeline, std::vector<XRPermutationElementArgument>& permutationDefinition)
{
	uint32_t const permutationHash = CalcPermutationHash(permutationDefinition);
	assert(_pipelineLookup.find(permutationHash) == _pipelineLookup.end());

	_pipelineLookup[permutationHash] = pipeline;
}

void XRPipelineGroup::SetDefaultPermutation(std::vector<XRPermutationElementArgument> const& elementArguments)
{
	_defaultFingerprint = elementArguments;
	_defaultHash = CalcPermutationHash(elementArguments);
}

uint32_t XRPipelineGroup::CalcPermutationHash(std::vector<XRPermutationElementArgument> const& elementArguments)
{
	std::vector<uint16_t> permutationFingerprint;
	permutationFingerprint.resize(_createInfo->_permutationLayout._numEnums, 0);

	uint32_t numElementArguments = static_cast<uint32_t>(elementArguments.size());
	for (uint32_t i = 0; i < numElementArguments; ++i)
	{
		XRPermutationElementArgument const& arg = elementArguments[i];
		permutationFingerprint[arg._id] = arg._value;
	}

	return GetHash(permutationFingerprint.data(), sizeof(uint16_t) * _createInfo->_permutationLayout._numEnums);
}

#pragma endregion XRPipelineGroup

#pragma region XRPipelineManager

bool XRPipelineManager::CreatePipeline(XRPipelineCreateInfo const& createInfo)
{
	_pipelineCreateInfos.push_back(createInfo);
	XRPipelineCreateInfo& pipelineCreateInfo = _pipelineCreateInfos.back();

	XRPipelineGroup* pipelineGroup = new XRPipelineGroup(pipelineCreateInfo);
	_pipelines.insert({ pipelineCreateInfo._name, pipelineGroup });

	std::vector<uint32_t> roundSizes;
	std::vector<XRPipelineStateDescription> pipelineDescriptions;

	XRPermutationBase::Layout& permutationLayout = pipelineCreateInfo._permutationLayout;
	uint32_t const numPermutationElements = permutationLayout._numEnums;
	roundSizes.resize(numPermutationElements + 1ull);
	uint32_t numAllPermutations = 1;
	{
		for (uint32_t i = 0; i < numPermutationElements; ++i)
		{
			XRPermutationElementInfo const& elementInfo = permutationLayout._enums[i];

			roundSizes[i] = numAllPermutations;
			numAllPermutations *= elementInfo._count;
		}
		roundSizes[numPermutationElements] = numAllPermutations;

		pipelineDescriptions.reserve(numAllPermutations);
	}

	std::vector<XRPermutationElementArgument> permutationElements;
	{
		permutationElements.resize(numPermutationElements);

		for (uint32_t i = 0; i < numPermutationElements; ++i)
		{
			XRPermutationElementInfo const& elementInfo = permutationLayout._enums[i];
			permutationElements[i]._id = i;
			permutationElements[i]._value = 0;
		}
	}

	// Permute pipelines
	{
		uint32_t numAvailablePermutations = 0;
		bool doCreatePipeline = true;

		pipelineDescriptions.reserve(numAllPermutations);

		for (uint32_t i = 0; i < numAllPermutations; ++i)
		{
			for (uint32_t j = 0; j < numPermutationElements; ++j)
			{
				XRPermutationElementInfo const& elementInfo = permutationLayout._enums[j];
				permutationElements[j]._value = i / roundSizes[j] % elementInfo._count;

			}

			if (doCreatePipeline == true)
				pipelineDescriptions.resize(numAvailablePermutations + 1ull);
			pipelineDescriptions[numAvailablePermutations] = pipelineCreateInfo._description;

			doCreatePipeline = pipelineCreateInfo._permute(pipelineDescriptions[numAvailablePermutations], permutationElements);

			XRPipeline* pipeline = nullptr;
			if (doCreatePipeline == true)
			{
				pipeline = xrCreatePipeline(&pipelineDescriptions[numAvailablePermutations]);
				doCreatePipeline = (pipeline != nullptr);
			}

			if (doCreatePipeline == true)
			{
				pipelineGroup->AddPipelineWithPermutation(pipeline, permutationElements);
				++numAvailablePermutations;
			}
		}

		pipelineDescriptions.shrink_to_fit();
	}

	return true;
}

#pragma endregion XRPipelineManager

#pragma region XRRenderPassManager

template<typename FPType>
struct FuncPointerAssigner
{
	union {
		FPType _fpTypedAddress;
		void* _fpUntypedAddress;
	};
};

bool XRWorkPassManager::RegisterWorkPassGenerator(std::string&& string, void* fpGnerator)
{
	// VS 비표준 구현은 이를 static casting으로 대입하는 것이 가능했지만, void*를 function pointer로 전달하는 것을 일반적으로는 용납할 수 없기 때문에
	// union을 통해 address를 강제로 전달합니다. 사실상 reinterpret와 다를 건 없습니다.
	FuncPointerAssigner<XRPFN_GENERATE_RENDERPASS> fpGeneratorAssigner;
	fpGeneratorAssigner._fpUntypedAddress = fpGeneratorAddress;
	
	XRWorkPassBase* workPass = fpGeneratorAssigner._fpTypedAddress();
	auto result = _workPasses.insert({ string, { workPass, fpGeneratorAssigner._fpTypedAddress } });
	return result.second;
}

#pragma endregion XRRenderPassManager

#pragma region XRCommandFootprint

uint32_t XRCommandFootprint::MakeHash() const
{
	return GetHash(_steps.data(), _steps.size() * sizeof(_steps[0]));
}

void XRCommandFootprint::Transcribe(XRCommandBuffer* commandBuffer) const
{
	for (uint32_t c = 0; c < _capturedCommands.size(); ++c)
	{
		_capturedCommands[c](commandBuffer);
	}
}

#pragma endregion XRCommandFootprint