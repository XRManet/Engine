#include "stdafx.h"

#include "XRHash.h"
#include "XRPipeline.h"

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

	uint32_t numElementArguments = elementArguments.size();
	for (uint32_t i = 0; i < numElementArguments; ++i)
	{
		XRPermutationElementArgument const& arg = elementArguments[i];
		permutationFingerprint[arg._id] = arg._value;
	}

	return GetHash(permutationFingerprint.data(), sizeof(uint16_t) * _createInfo->_permutationLayout._numEnums);
}

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

			numAllPermutations *= elementInfo._count;
			roundSizes[i] = numAllPermutations;
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

		for (uint32_t i = 0; i < numAllPermutations; ++i)
		{
			for (uint32_t j = 0; j < numPermutationElements; ++j)
				permutationElements[j]._value = i % roundSizes[j];

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
	}

	return true;
}

bool XRRenderPassManager::RegisterRenderPassGenerator(std::string&& string, void* fpGnerator)
{
	auto Generate = static_cast<XRPFN_GENERATE_RENDERPASS>(fpGnerator);
	XRRenderPassBase* renderPass = Generate();
	auto result = _renderPasses.insert({ string, { renderPass, Generate } });
	return result.second;
}
