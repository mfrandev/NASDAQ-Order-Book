#ifndef STOCK_DIRECTORY_H_
#define STOCK_DIRECTORY_H_

#include <cstdint>
#include <cstring>

#include <string_utils.hpp>
#include <endian_utils.hpp>
#include <MessageConstants.h>

#include <iostream>

struct StockDirectory {
    // std::string stock;
    // char marketCategory;
    // char financialStatusIndicator;
    // char roundLotsOnly;
    // char issueClassification;
    // char authenticity;
    // char shortSaleThresholdIndicator;
    // char ipoFlag;
    // char luldReferencePriceTier;
    // char etpFlag;
    // char inverseIndicator;
    // uint16_t issueSubType;
    // uint32_t roundLotSize;
    // uint32_t etpLeverageFactor;
};

/**
 * Right now, we only want to build the association stockLocate -> symbol. 
 * Assume that "stockBuffer" is the properly indexed position in the symbol map array
 */
StockDirectory parseStockDirectoryBody(char stockBuffer[MessageFieldSizes::STOCK_SIZE], const char buffer[MAX_BUFFER_SIZE]) {
    // size_t offset = 0;
    std::memcpy(stockBuffer, buffer, MessageFieldSizes::STOCK_SIZE);
    // std::cout << "Parsed " << std::string(stockBuffer, MessageFieldSizes::STOCK_SIZE) << std::endl;
    // offset += MessageFieldSizes::STOCK_SIZE;
    // char marketCategory = buffer[offset];
    // offset += MessageFieldSizes::MARKET_CATEGORY_SIZE;
    // char financialStatusIndicator = buffer[offset];
    // offset += MessageFieldSizes::FINANCIAL_STATUS_INDICATOR_SIZE;
    // uint32_t roundLotSize = toHostEndianUpTo64(&buffer[offset], MessageFieldSizes::ROUND_LOT_SIZE_SIZE);
    // offset += MessageFieldSizes::ROUND_LOT_SIZE_SIZE;
    // char roundLotsOnly = buffer[offset];
    // offset += MessageFieldSizes::ROUND_LOTS_ONLY_SIZE;
    // char issueClassification = buffer[offset];
    // offset += MessageFieldSizes::ISSUE_CLASSIFICATION_SIZE;
    // uint16_t issueSubType = toHostEndianUpTo64(&buffer[offset], MessageFieldSizes::ISSUE_SUB_TYPE_SIZE);
    // offset += MessageFieldSizes::ISSUE_SUB_TYPE_SIZE;
    // char authenticity = buffer[offset];
    // offset += MessageFieldSizes::AUTHENTICITY_SIZE;
    // char shortSaleThresholdIndicator = buffer[offset];
    // offset += MessageFieldSizes::SHORT_SALE_THRESHOLD_INDICATOR_SIZE;
    // char ipoFlag = buffer[offset];
    // offset += MessageFieldSizes::IPO_FLAG_SIZE;
    // char luldReferencePriceTier = buffer[offset];
    // offset += MessageFieldSizes::LULD_REFERENCE_PRICE_TIER_SIZE;
    // char etpFlag = buffer[offset];
    // offset += MessageFieldSizes::ETP_FLAG_SIZE;
    // uint32_t etpLeverageFactor = toHostEndianUpTo64(&buffer[offset], MessageFieldSizes::ETP_LEVERAGE_FACTOR_SIZE);
    // offset += MessageFieldSizes::ETP_LEVERAGE_FACTOR_SIZE;
    // char inverseIndicator = buffer[offset];
    return StockDirectory{};
}

#endif // STOCK_DIRECTORY_H_