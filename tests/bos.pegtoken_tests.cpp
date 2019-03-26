#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>
#include "eosio.system_tester.hpp"

#include "Runtime/Runtime.h"

#include <fc/variant_object.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;

class bos_pegtoken_tester : public tester {
public:

   bos_pegtoken_tester() {
      produce_blocks( 2 );

      create_accounts( { N(alice), N(bob), N(eosio.token),N(btc.bos) } );
      produce_blocks( 2 );

      set_code( N(btc.bos), contracts::pegtoken_wasm() );
      set_abi( N(btc.bos), contracts::pegtoken_abi().data() );

      produce_blocks();

      const auto& accnt = control->db().get<account_object,by_name>( N(btc.bos) );
      abi_def abi;
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
      abi_ser.set_abi(abi, abi_serializer_max_time);
   }

   action_result push_action( const account_name& signer, const action_name &name, const variant_object &data ) {
      string action_type_name = abi_ser.get_action_type(name);

      action act;
      act.account = N(btc.bos);
      act.name    = name;
      act.data    = abi_ser.variant_to_binary( action_type_name, data,abi_serializer_max_time );

      return base_tester::push_action( std::move(act), uint64_t(signer));
   }

   fc::variant get_personal( const name act,const string key ) {
      name key_name{key};
      vector<char> data = get_row_by_account( N(btc.bos), act, N(personaldata), key_name.value );
      return abi_ser.binary_to_variant( "personal", data, abi_serializer_max_time );
   }

   abi_serializer abi_ser;
};

BOOST_AUTO_TEST_SUITE(bos_pegtoken_tests)

BOOST_FIXTURE_TEST_CASE( create_pegtoken, bos_pegtoken_tester ) try {
//    //only owner can call setpersonal
//    BOOST_REQUIRE_EQUAL(error("missing authority of alice"),
//                         push_action( N(bob), N(setpersonal), mvo()
//                                           ("account", "alice")
//                                           ("key", "favorfood")
//                                           ("value", "pizza"))
//    );

//    //key length check
//    BOOST_REQUIRE_EQUAL(wasm_assert_msg("key should be less than 12"),
//                         push_action( N(alice), N(setpersonal), mvo()
//                                           ("account", "alice")
//                                           ("key", "tooooooooooolongkey")
//                                           ("value", "somevalue"))
//    );

//    //value length check
//    string* long_value=new string(1025,'a');
//    BOOST_REQUIRE_EQUAL(wasm_assert_msg("value should be less than 1024"),
//                         push_action( N(alice), N(setpersonal), mvo()
//                                           ("account", "alice")
//                                           ("key", "somekey")
//                                           ("value",*long_value))
//    );

//    //setpersonal should success,and check result
//    BOOST_REQUIRE_EQUAL(success(),
//                         push_action( N(alice), N(setpersonal), mvo()
//                                           ("account", "alice")
//                                           ("key", "favorfood")
//                                           ("value","pizza")
//                         )
//    );
//    auto data=get_personal(N(alice),"favorfood");
//    BOOST_REQUIRE_EQUAL(0,data["readablekey"].as_string().find("favorfood"));
//    BOOST_REQUIRE_EQUAL(0,data["value"].as_string().find("pizza"));

//    //setpersonal to other value check
//    BOOST_REQUIRE_EQUAL(success(),
//                         push_action( N(alice), N(setpersonal), mvo()
//                                           ("account", "alice")
//                                           ("key", "favorfood")
//                                           ("value","noodles")
//                         )
//    );
//    data=get_personal(N(alice),"favorfood");
//    BOOST_REQUIRE_EQUAL(0,data["readablekey"].as_string().find("favorfood"));
//    BOOST_REQUIRE_EQUAL(0,data["value"].as_string().find("noodles"));

//    //sethomepage check
//    //long url check
//    string *long_url=new string(257,'a');
//    BOOST_REQUIRE_EQUAL(wasm_assert_msg("url is too long"),
//                         push_action( N(alice), N(sethomepage), mvo()
//                                       ("account", "alice")
//                                       ("url",*long_url))
//    );

//    //url prefix check
//    string url="abcdefg";
//    BOOST_REQUIRE_EQUAL(wasm_assert_msg("illegal url"),
//                         push_action( N(alice), N(sethomepage), mvo()
//                                           ("account", "alice")
//                                           ("url",url))
//    );

//    //sethomepage should success,and check result
//    BOOST_REQUIRE_EQUAL(success(),
//                        push_action( N(alice), N(sethomepage), mvo()
//                                           ("account", "alice")
//                                           ("url","http://somepage.com")
//                        )
//    );
//    data=get_personal(N(alice),"homepage");
//    BOOST_REQUIRE_EQUAL(0,data["value"].as_string().find("http://somepage.com"));
} FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()