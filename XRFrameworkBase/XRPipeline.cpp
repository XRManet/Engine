#include "stdafx.h"

#include "XRPipeline.h"

void XRPipelineGroup::AddPipelineWithPermutation(XRPipeline* pipeline, std::vector<XRPermutationElementArgument>& permutationDefinition)
{

}

void XRPipelineGroup::GetCurrentPermutation(std::vector<XRPermutationElementArgument>& elementArguments)
{
}

void XRPipelineGroup::SetCurrentPermutation(std::vector<XRPermutationElementArgument> const& elementArguments)
{
}

bool XRPipelineManager::CreatePipeline(XRPipelineCreateInfo const& createInfo)
{
	_pipelineCreateInfos.emplace_back(std::move(createInfo));

	XRPipelineGroup* pipelineGroup = new XRPipelineGroup(_pipelineCreateInfos.size() - 1);
	XRPipelineCreateInfo& pipelineCreateInfo = _pipelineCreateInfos.back();
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
