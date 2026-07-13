#pragma once
#include <string>
#include <orm/Schema.hpp>
#include "Topic.hpp"

namespace broker {
namespace models {

struct Event {
    int id;
    int topic_id;
    std::string payload;
};

} // namespace models
} // namespace broker

namespace orm {
template <>
struct Schema<broker::models::Event> {
    static constexpr std::string_view table_name = "events";
    static constexpr auto columns = std::make_tuple(
        make_column("id", &broker::models::Event::id),
        make_column("topic_id", &broker::models::Event::topic_id),
        make_column("payload", &broker::models::Event::payload)
    );
    static constexpr auto foreign_keys = std::make_tuple(
        make_foreign_key(&broker::models::Event::topic_id, &broker::models::Topic::id)
    );
};
} // namespace orm
