//#include <json2pb.h>


#include <mesos/mesos.hpp>

#include <stout/os.hpp>

#include "pbjson.hpp"
#include "json_source.hpp"

#include "json_source.pb.h"

namespace mesos {
namespace serenity {



void JsonSource::RunTests(const std::string& jsonSource)
{
  Try<mesos::FixtureResourceUsage> usages = JsonSource::ReadJson(jsonSource);
  if (usages.isError()){
    LOG(ERROR) << "JsonSource failed: " << usages.error() << std::endl;
  }

  for(auto itr = usages.get().resource_usage().begin();
      itr != usages.get().resource_usage().end();
      itr++)
  {
    produce(*itr);
  }

  return;
}

const Try<mesos::FixtureResourceUsage> JsonSource::ReadJson(
    const std::string& relativePath)
{
  Try<std::string> content = os::read(relativePath);
  if (content.isError()) {
    return Error("Read error: " + content.error());
  } else if (!content.isSome()){
    return Error("Readed file is none");
  }

  std::string err;
  mesos::FixtureResourceUsage ru;
  int reply = pbjson::json2pb(content.get(), &ru, err);
  if (reply != 0){
    Try<std::string> emsg = strings::format(
        "Error during json deserialization| errno: %d | err: %s", reply, err);
    return Error(emsg.get());
  }

  return ru;
}

} //namespace serenity
} //namespace mesos
