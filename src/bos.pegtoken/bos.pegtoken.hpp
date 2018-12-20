#pragma once

#include "types.hpp"

#define MULTI_INDEX_1(table) \
    using table##s = eosio::multi_index<#table "s"##_n, table##_ts>;

#define MULTI_INDEX_2(table, index_type, index_name)                \
    using table##s = eosio::multi_index<#table "s"##_n, table##_ts, \
        indexed_by<#index_name##_n, const_mem_fun<table##_ts, index_type, &table##_ts::by_##index_name>>>;

#define MULTI_INDEX_4(table,                                                                                 \
    index_type1, index_name1,                                                                                \
    index_type2, index_name2,                                                                                \
    index_type3, index_name3)                                                                                \
    using table##s = eosio::multi_index<#table "s"##_n, table##_ts,                                          \
        indexed_by<#index_name1##_n, const_mem_fun<table##_ts, index_type1, &table##_ts::by_##index_name1>>, \
        indexed_by<#index_name2##_n, const_mem_fun<table##_ts, index_type2, &table##_ts::by_##index_name2>>, \
        indexed_by<#index_name3##_n, const_mem_fun<table##_ts, index_type3, &table##_ts::by_##index_name3>>>;

namespace eosio {

class[[eosio::contract("bos.pegtoken")]] pegtoken : public contract
{
public:
    using contract::contract;

    [[eosio::action]] void create(name issuer, symbol sym);

    [[eosio::action]] void init(symbol_code sym_code, string organization, string website, name acceptor);

    [[eosio::action]] void update(symbol_code sym_code, string organization, string website);

    [[eosio::action]] void setlimit(asset max_limit, asset min_limit, asset total_limit, uint64_t frequency_limit, uint64_t interval_limit);

    [[eosio::action]] void setauditor(symbol_code sym_code, name auditor, string action);

    [[eosio::action]] void setfee(double fee_rate, asset min_fee);

    [[eosio::action]] void issue(name to, asset quantity, string memo);

    [[eosio::action]] void retire(name to, asset quantity, string memo);

    [[eosio::action]] void applicant(symbol_code sym_code, string action, name applicant);

    [[eosio::action]] void applyaddr(name applicant, symbol_code sym_code, name to);

    [[eosio::action]] void assignaddr(symbol_code sym_code, name to, string address);

    [[eosio::action]] void withdraw(name from, string to, asset quantity, string memo);

    [[eosio::action]] void deposit(name to, asset quantity, string memo);

    [[eosio::action]] void transfer(name from, name to, asset quantity, string memo);

    [[eosio::action]] void clear(symbol_code sym_code, uint64_t num);

    [[eosio::action]] void feedback(symbol_code sym_code, transaction_id_type trx_id, string remote_trx_id, string memo);

    [[eosio::action]] void rollback(symbol_code sym_code, transaction_id_type trx_id, string memo);

    [[eosio::action]] void setacceptor(symbol_code sym_code, name acceptor);

    [[eosio::action]] void setdelay(symbol_code sym_code, uint64_t delayday);

    [[eosio::action]] void lockall(symbol_code sym_code, name auditor);

    [[eosio::action]] void unlockall(symbol_code sym_code, name auditor);

    [[eosio::action]] void approve(symbol_code sym_code, name auditor, transaction_id_type trx_id, string memo);

    [[eosio::action]] void unapprove(symbol_code sym_code, name auditor, transaction_id_type trx_id, string memo);

    [[eosio::action]] void sendback(name auditor, transaction_id_type trx_id, name to, asset quantity, string memo);

    [[eosio::action]] void rmwithdraw(uint64_t id, symbol_code sym_code);

private:
    void verify_address(symbol_code sym_code, string address);
    void add_balance(name owner, asset value, name ram_payer);
    void sub_balance(name owner, asset value);

    MULTI_INDEX_1(symbol)
    MULTI_INDEX_1(applicant)
    MULTI_INDEX_2(rchrgaddr, uint64_t, address)
    MULTI_INDEX_1(operate)
    MULTI_INDEX_4(withdraw, fixed_bytes<32>, trxid, uint128_t, delindex, uint128_t, queindex)
    MULTI_INDEX_2(deposit, uint64_t, delindex)
    MULTI_INDEX_1(statistic)
    MULTI_INDEX_1(account)
    MULTI_INDEX_1(stat)
    MULTI_INDEX_1(auditor)
};

} // namespace eosio