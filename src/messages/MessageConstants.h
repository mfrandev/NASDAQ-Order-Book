#ifndef MESSAGE_CONSTANTS_H_
#define MESSAGE_CONSTANTS_H_

#include <cstddef>

constexpr size_t NUMBER_OF_BYTES_FOR_HEADER_CHUNK          = 13;
constexpr size_t NUMBER_OF_BYTES_OFFSET_FOR_HEADER_CHUNK   = 2;
constexpr size_t MAX_BUFFER_SIZE = 40;

constexpr char PRINTABLE = 'Y';
constexpr char NON_PRINTABLE = 'N';

namespace StockDirectoryConstants {
    constexpr char NASDAQ_GLOBAL_SELECT_MARKET = 'Q';
    constexpr char NASDAQ_GLOBAL_MARKET = 'G';
    constexpr char NASDAQ_CAPITAL_MARKET = 'S';
    constexpr char NYSE = 'N';
    constexpr char NYSE_AMERICAN = 'A';
    constexpr char NYSE_ARCA = 'P';
    constexpr char BATS_Z_EXCHANGE = 'Z';
    constexpr char INVESTORS_EXCHANGE_LLC = 'V';
    constexpr char NOT_AVAILABLE = ' ';
    constexpr char DEFICIENT = 'D';
    constexpr char DELINQUENT = 'E';
    constexpr char BANKRUPT = 'Q';
    constexpr char SUSPENDED = 'S';
    constexpr char DEFICIENT_AND_BANKRUPT = 'G';
    constexpr char DEFICIENT_AND_DELINQUENT = 'H';
    constexpr char DELINQUENT_AND_BANKRUPT = 'K';
    constexpr char DEFICIENT_DELINQUENT_AND_BANKRUPT = 'K';
    constexpr char CREATIONS_REDEMPTIONS_SUSPENDED_FOR_EXCHANGE_TRADED_PRODUCT = 'C';
    constexpr char NORMAL = 'N';
    constexpr char NASDAQ_SYSTEM_ONLY_ACCEPTS_ROUND_LOTS = 'Y';
    constexpr char NASDAQ_SYSTEM_NO_ORDER_SIZE_RESTRICTIONS = 'N';
    constexpr char LIVE_PRODUCTION = 'P';
    constexpr char TEST = 'T';
    constexpr char ISSUE_RESTRICTED_UNDER_RULE_203_b_3 = 'Y';
    constexpr char ISSUE_NOT_RESTRICTED = 'N';
    constexpr char THRESHOLD_INDICATOR_NOT_AVAILABLE = ' ';
    constexpr char NASDAQ_LISTED_INSTRUMENT_IS_NEW_IPO = 'Y';
    constexpr char NASDAQ_LISTED_INSTRUMENT_NOT_NEW_IPO = 'N';
    constexpr char TIER_1_NMS_STOCKS_AND_SELECT_ETP = '1';
    constexpr char TIER_2_NMS_STOCKS = '2';
    constexpr char INSTRUMENT_IS_AN_ETP = 'Y';
    constexpr char INSTRUMENT_IS_NOT_AN_ETP = 'N';
    constexpr char ETP_IS_AN_INVERSE_ETP = 'Y';
    constexpr char ETP_IS_NOT_AN_INVERSE_ETP = 'N';
};

namespace SystemEventCodes {
    constexpr char START_OF_MESSAGES = 'O';
    constexpr char START_OF_SYSTEM_HOURS = 'S';
    constexpr char START_OF_MARKET_HOURS = 'Q';
    constexpr char END_OF_MARKET_HOURS = 'M';
    constexpr char END_OF_SYSTEM_HOURS = 'E';
    constexpr char END_OF_MESSAGES = 'C';
};

namespace MessageFieldSizes {

    constexpr size_t MESSAGE_TYPE_SIZE = 1;
    constexpr size_t STOCK_LOCATE_SIZE = 2;
    constexpr size_t TRACKING_NUMBER_SIZE = 2;
    constexpr size_t TIMESTAMP_SIZE = 6;

    constexpr size_t ORDER_REFERENCE_NUMBER_SIZE = 8;
    constexpr size_t BUY_SELL_INDICATOR_SIZE = 1;
    constexpr size_t STOCK_SIZE = 8;
    constexpr size_t SHARES_SIZE = 4;
    constexpr size_t PRICE_SIZE = 4;
    constexpr size_t ATTRIBUTION_SIZE = 4;
    constexpr size_t MATCH_NUMBER_SIZE = 8;
    constexpr size_t CROSS_PRICE_SIZE = 4;
    constexpr size_t SHARES_IN_TRADE_MESSAGE_SIZE = 8;
    constexpr size_t CANCELLED_SHARES_SIZE = 4;
    constexpr size_t EXECUTED_SHARES_SIZE = 4;
    constexpr size_t PRINTABLE_SIZE = 1;
    constexpr size_t EXECUTION_PRICE_SIZE = 4;
    constexpr size_t TRADING_STATE_SIZE = 1;
    constexpr size_t RESERVED_SIZE = 1;
    constexpr size_t RESASON_SIZE = 4;
    constexpr size_t MARKET_CATEGORY_SIZE = 1;
    constexpr size_t FINANCIAL_STATUS_INDICATOR_SIZE = 1;
    constexpr size_t ROUND_LOT_SIZE_SIZE = 4;
    constexpr size_t ROUND_LOTS_ONLY_SIZE = 1;
    constexpr size_t ISSUE_CLASSIFICATION_SIZE = 1;
    constexpr size_t ISSUE_SUB_TYPE_SIZE = 2;
    constexpr size_t AUTHENTICITY_SIZE = 1;
    constexpr size_t SHORT_SALE_THRESHOLD_INDICATOR_SIZE = 1;
    constexpr size_t IPO_FLAG_SIZE = 1;
    constexpr size_t LULD_REFERENCE_PRICE_TIER_SIZE = 1;
    constexpr size_t ETP_FLAG_SIZE = 1;
    constexpr size_t ETP_LEVERAGE_FACTOR_SIZE = 4;
    constexpr size_t INVERSE_INDICATOR_SIZE = 1;
    
};

/**
 * Message Type identifiers for all of the messages from the protocol spec
 */ 
namespace MessageTypes {
    constexpr char MESSAGE_TYPE_SYSTEM_EVENT                = 'S'; // 1.1

    constexpr char MESSAGE_TYPE_STOCK_DIRECTORY             = 'R'; // 1.2.1
    constexpr char MESSAGE_TYPE_STOCK_TRADING_ACTION        = 'H'; // 1.2.2
    constexpr char MESSAGE_TYPE_REG_SHO                     = 'Y'; // 1.2.3
    constexpr char MESSAGE_TYPE_MARKET_PARTICIPANT_POSITION = 'L'; // 1.2.4
    constexpr char MESSAGE_TYPE_MWCB_DECLINE_LEVEL          = 'V'; // 1.2.5.1
    constexpr char MESSAGE_TYPE_MWCB_STATUS                 = 'W'; // 1.2.5.2
    constexpr char MESSAGE_TYPE_QUOTING_PERIOD_UPDATE       = 'K'; // 1.2.6
    constexpr char MESSAGE_TYPE_LULD_AUCTION_COLLAR         = 'J'; // 1.2.7
    constexpr char MESSAGE_TYPE_OPERATIONAL_HALT            = 'h'; // 1.2.8

    constexpr char MESSAGE_TYPE_ADD_ORDER_NO_MPID           = 'A'; // 1.3.1
    constexpr char MESSAGE_TYPE_ADD_ORDER_WITH_MPID         = 'F'; // 1.3.2

    constexpr char MESSAGE_TYPE_ORDER_EXECUTED              = 'E'; // 1.4.1
    constexpr char MESSAGE_TYPE_ORDER_EXECUTED_WITH_PRICE   = 'C'; // 1.4.2
    constexpr char MESSAGE_TYPE_ORDER_CANCELLED             = 'X'; // 1.4.3
    constexpr char MESSAGE_TYPE_ORDER_DELETE                = 'D'; // 1.4.4
    constexpr char MESSAGE_TYPE_ORDER_REPLACE               = 'U'; // 1.4.5

    constexpr char MESSAGE_TYPE_TRADE_NON_CROSS             = 'P'; // 1,5,1
    constexpr char MESSAGE_TYPE_TRADE_CROSS                 = 'Q'; // 1.5.2
    constexpr char MESSAGE_TYPE_TRADE_BROKEN                = 'B'; // 1.5.3

    constexpr char MESSAGE_TYPE_NOII                        = 'I'; // 1.6

    constexpr char MESSAGE_TYPE_RPII                        = 'N'; // 1.7

    constexpr char MESSAGE_TYPE_DLCR_PRICE_DISCOVERY        = 'O'; // 1.8

    constexpr char MESSAGE_TYPE_SENTINEL_MESSAGE            = '1'; // Terminate consumer thread
};

/**
 * Message size in bytes for all messages in the protocol spec, EXCLUDING THE HEADER
 */
namespace MessageSizes {
    constexpr size_t MESSAGE_HEADER_SIZE                      = 11; // COMMON MESSAGE HEADER

    constexpr size_t MESSAGE_SIZE_SYSTEM_EVENT                = 1; // 1.1

    constexpr size_t MESSAGE_SIZE_STOCK_DIRECTORY             = 28; // 1.2.1
    constexpr size_t MESSAGE_SIZE_STOCK_TRADING_ACTION        = 14; // 1.2.2
    constexpr size_t MESSAGE_SIZE_REG_SHO                     = 9; // 1.2.3
    constexpr size_t MESSAGE_SIZE_MARKET_PARTICIPANT_POSITION = 15; // 1.2.4
    constexpr size_t MESSAGE_SIZE_MWCB_DECLINE_LEVEL          = 24; // 1.2.5.1
    constexpr size_t MESSAGE_SIZE_MWCB_STATUS                 = 1; // 1.2.5.2
    constexpr size_t MESSAGE_SIZE_QUOTING_PERIOD_UPDATE       = 17; // 1.2.6
    constexpr size_t MESSAGE_SIZE_LULD_AUCTION_COLLAR         = 24; // 1.2.7
    constexpr size_t MESSAGE_SIZE_OPERATIONAL_HALT            = 10; // 1.2.8

    constexpr size_t MESSAGE_SIZE_ADD_ORDER_NO_MPID           = 25; // 1.3.1
    constexpr size_t MESSAGE_SIZE_ADD_ORDER_WITH_MPID         = 29; // 1.3.2

    constexpr size_t MESSAGE_SIZE_ORDER_EXECUTED              = 20; // 1.4.1
    constexpr size_t MESSAGE_SIZE_ORDER_EXECUTED_WITH_PRICE   = 25; // 1.4.2
    constexpr size_t MESSAGE_SIZE_ORDER_CANCELLED             = 12; // 1.4.3
    constexpr size_t MESSAGE_SIZE_ORDER_DELETE                = 8; // 1.4.4
    constexpr size_t MESSAGE_SIZE_ORDER_REPLACE               = 24; // 1.4.5

    constexpr size_t MESSAGE_SIZE_TRADE_NON_CROSS             = 33; // 1,5,1
    constexpr size_t MESSAGE_SIZE_TRADE_CROSS                 = 29; // 1.5.2
    constexpr size_t MESSAGE_SIZE_TRADE_BROKEN                = 8; // 1.5.3

    constexpr size_t MESSAGE_SIZE_NOII                        = 39; // 1.6

    constexpr size_t MESSAGE_SIZE_RPII                        = 9; // 1.7

    constexpr size_t MESSAGE_SIZE_DLCR_PRICE_DISCOVERY        = 37; // 1.8
};

#endif // MESSAGE_CONSTANTS_H_