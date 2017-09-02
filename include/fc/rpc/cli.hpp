#pragma once
#include <fc/io/stdio.hpp>
#include <fc/io/json.hpp>
#include <fc/io/buffered_iostream.hpp>
#include <fc/io/sstream.hpp>
#include <fc/rpc/api_connection.hpp>
#include <fc/thread/thread.hpp>

#include <iostream>


namespace fc { namespace rpc {
   
using result_formatter = std::map<std::string, std::function<std::string(fc::variant, const fc::variants&)> >;

   /**
    *  Provides a simple wrapper for RPC calls to a given interface.
    */
   class cli : public api_connection
   {
      public:
         ~cli();

         virtual variant send_call( api_id_type api_id, string method_name, variants args = variants() );
         virtual variant send_callback( uint64_t callback_id, variants args = variants() );
         virtual void    send_notice( uint64_t callback_id, variants args = variants() );

         void start();
         void stop();
         void wait();
         void format_result( const string& method, std::function<string(variant,const variants&)> formatter);

         virtual void getline( const fc::string& prompt, fc::string& line );

         void set_prompt( const string& prompt );
         
         virtual void exec_command (
            const std::string & command,
            std::vector < std::pair < std::string, std::string > > & commands_output
         );

      private:
         void run();
         std::string _prompt = ">>>";
         result_formatter _result_formatters;
         fc::future<void> _run_complete;

   };
} } 
