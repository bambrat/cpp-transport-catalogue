#include "serialization.h"

namespace tc_protobuf {
	using namespace transport::catalogue;
	using namespace transport::domain;
	using namespace transport::router;
	using namespace map_renderer;
	using namespace std::literals;

	ColorProto convertColorToColorProto(const svg::Color& tc_color) {
		ColorProto color_proto;

		if (std::holds_alternative<std::monostate>(tc_color)) {
			color_proto.set_none(true);
		}
		else if (std::holds_alternative<svg::Rgb>(tc_color)) {
			svg::Rgb rgb = std::get<svg::Rgb>(tc_color);

			color_proto.mutable_rgb()->set_red_(rgb.red);
			color_proto.mutable_rgb()->set_green_(rgb.green);
			color_proto.mutable_rgb()->set_blue_(rgb.blue);
		}
		else if (std::holds_alternative<svg::Rgba>(tc_color)) {
			svg::Rgba rgba = std::get<svg::Rgba>(tc_color);

			color_proto.mutable_rgba()->set_red_(rgba.red);
			color_proto.mutable_rgba()->set_green_(rgba.green);
			color_proto.mutable_rgba()->set_blue_(rgba.blue);
			color_proto.mutable_rgba()->set_opacity_(rgba.opacity);
		}
		else if (std::holds_alternative<std::string>(tc_color)) {
			color_proto.set_string_color(std::get<std::string>(tc_color));
		}
		return color_proto;
	}

	int getСachePointer(const std::deque<std::string_view>& cache, const std::string_view& find_name)
	{
		auto stop_it = std::find_if(cache.cbegin(), cache.cend(), [&](auto& f_name) {
			return find_name == f_name;
			});
		return std::distance(cache.cbegin(), stop_it);
	}

	void saveData(transport::catalogue::TransportCatalogue& db, map_renderer::RenderSettings& rs, transport::router::RouterSettings& routs, const std::string& file)
	{
		DataToSaveProto serialize_transport_catalogue;

		auto& stops = db.getAllStops();
		auto& buses = db.getAllBuses();

		std::deque<std::string_view> cache_stops;

		for (auto& stop : stops) {
			cache_stops.push_back(stop.second->name);
		}

		for (auto& stop : stops) {
			StopProto sp;
			sp.set_name(stop.second->name);
			sp.set_latitude(stop.second->coord.lat);
			sp.set_longitude(stop.second->coord.lng);
			auto& nsd = stop.second->next_stop_dist;

			for (auto& [stop, size] : nsd) {
				DistanceProto dp;
				dp.set_next_stop(getСachePointer(cache_stops, stop->name));
				dp.set_distance(size);
				*sp.add_distance() = std::move(dp);
			}

			*serialize_transport_catalogue.add_stops() = std::move(sp);
		}

		for (auto& bus : buses) {
			BusProto bp;
			bp.set_name(bus.second->name);

			for (auto bus_stop : bus.second->stops) {
				bp.add_stops(getСachePointer(cache_stops, bus_stop->name));
			}

			bp.set_is_roundtrip(bus.second->is_roundtrip);
			*serialize_transport_catalogue.add_buses() = std::move(bp);
		}

		RenderSettingsProto rsp;
		rsp.set_width(rs.width_);
		rsp.set_height(rs.height_);
		rsp.set_padding(rs.padding_);
		rsp.set_line_width(rs.line_width_);
		rsp.set_stop_radius(rs.stop_radius_);
		rsp.set_bus_label_font_size(rs.bus_label_font_size_);
		rsp.set_bus_label_offset_x(rs.bus_label_offset_.first);
		rsp.set_bus_label_offset_y(rs.bus_label_offset_.second);
		rsp.set_stop_label_font_size(rs.stop_label_font_size_);
		rsp.set_stop_label_offset_x(rs.stop_label_offset_.first);
		rsp.set_stop_label_offset_y(rs.stop_label_offset_.second);
		rsp.set_underlayer_width(rs.underlayer_width_);

		*rsp.mutable_underlayer_color() = std::move(convertColorToColorProto(rs.underlayer_color_));

		for (auto& color : rs.color_palette_) {
			*rsp.add_color_palette() = std::move(convertColorToColorProto(color));
		}

		*serialize_transport_catalogue.mutable_render_settings() = std::move(rsp);

		RoutingSettingsProto rout_settings_proto;
		rout_settings_proto.set_bus_velocity(routs.velocity);
		rout_settings_proto.set_bus_wait_time(routs.wait_time);

		*serialize_transport_catalogue.mutable_routing_settings() = std::move(rout_settings_proto);

		std::ofstream out_file(file, std::ios::binary);
		serialize_transport_catalogue.SerializePartialToOstream(&out_file);
	}

	tc_protobuf::DataToSaveProto protoParseFromIstream(std::string file) {
		std::ifstream in_file(file, std::ios::binary);

		DataToSaveProto proto;
		bool done = proto.ParseFromIstream(&in_file);

		if (!done) {
			throw std::runtime_error("cannot parse serialized file from istream");
		}
		return proto;
	}

	transport::catalogue::TransportCatalogue loadCatalogueData(tc_protobuf::DataToSaveProto proto)
	{
		transport::catalogue::TransportCatalogue db;
		auto& stops_p = proto.stops();
		auto& buses_p = proto.buses();

		std::deque<std::string> cache_stops;
		for (auto& stop_p : stops_p) {
			cache_stops.push_back(stop_p.name());
		}

		for (auto& stop_p : stops_p) {
			Stop stop;
			stop.name = stop_p.name();
			stop.coord = { stop_p.latitude(),stop_p.longitude() };
			db.addStop(std::move(stop));
		}

		for (auto& stop_p : stops_p) {
			auto stopA = db.getStop(stop_p.name());

			for (auto& distance_proto : stop_p.distance()) {
				auto id = distance_proto.next_stop();
				auto distance = distance_proto.distance();
				auto stopB = db.getStop(cache_stops.at(id));
				db.addDistance(stopA, stopB, distance);
			}
		}

		for (auto& bus_p : buses_p) {
			Bus bus;
			bus.name = bus_p.name();

			for (auto stop_id : bus_p.stops()) {
				auto& name = cache_stops.at(stop_id);
				bus.stops_list.push_back(name);
			}
			bus.is_roundtrip = bus_p.is_roundtrip();
			db.addBus(std::move(bus));
		}
		return db;
	}

	svg::Color convertColorProtoToColor(const ColorProto& cp)
	{
		svg::Color color;

		if (cp.has_rgb()) {
			svg::Rgb rgb;
			rgb.red = cp.rgb().red_();
			rgb.green = cp.rgb().green_();
			rgb.blue = cp.rgb().blue_();

			color = rgb;
		}
		else if (cp.has_rgba()) {
			svg::Rgba rgba;

			rgba.red = cp.rgba().red_();
			rgba.green = cp.rgba().green_();
			rgba.blue = cp.rgba().blue_();
			rgba.opacity = cp.rgba().opacity_();

			color = rgba;
		}
		else {
			color = cp.string_color();
		}

		return color;
	}

	RenderSettings loadRenderSettings(tc_protobuf::DataToSaveProto proto)
	{
		RenderSettings rs;

		auto& rsp = proto.render_settings();
		rs.width_ = rsp.width();
		rs.height_ = rsp.height();
		rs.padding_ = rsp.padding();
		rs.line_width_ = rsp.line_width();
		rs.stop_radius_ = rsp.stop_radius();
		rs.bus_label_font_size_ = rsp.bus_label_font_size();
		rs.bus_label_offset_.first = rsp.bus_label_offset_x();
		rs.bus_label_offset_.second = rsp.bus_label_offset_y();
		rs.stop_label_font_size_ = rsp.stop_label_font_size();
		rs.stop_label_offset_.first = rsp.stop_label_offset_x();
		rs.stop_label_offset_.second = rsp.stop_label_offset_y();
		rs.underlayer_width_ = rsp.underlayer_width();
		rs.underlayer_color_ = std::move(convertColorProtoToColor(rsp.underlayer_color()));

		for (auto& color_p : rsp.color_palette()) {
			rs.color_palette_.push_back(std::move(convertColorProtoToColor(color_p)));
		}

		return rs;
	}

	RouterSettings loadRouterSettings(tc_protobuf::DataToSaveProto proto)
	{
		RouterSettings rs;
		rs.velocity = proto.routing_settings().bus_velocity();
		rs.wait_time = proto.routing_settings().bus_wait_time();
		return rs;
	}

} // end namespace tc_protobuf