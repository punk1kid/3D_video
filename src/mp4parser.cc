#include "mp4parser.h"
#include "source.h"

using namespace mp4parser;
using namespace diplomproject;

Mp4Data::Mp4Data(std::string_view mp4_path) : error_(0) {
	std::ifstream mp4_read_(mp4_path.data(), std::ios::binary);
	
	if (!mp4_read_.is_open()) {
		error_ = 1;
		return;
	}
	
	ReadHeaders(mp4_read_);
	ReadFtyp(mp4_read_);
	ReadMvhd(mp4_read_);
	ReadTkhd(mp4_read_);
	ReadMdhd(mp4_read_);
	ReadHdlr(mp4_read_);
	ReadVmhd(mp4_read_);
	ReadSmhd(mp4_read_);
	ReadDref(mp4_read_);
	ReadStsz(mp4_read_);
	ReadStts(mp4_read_);

	mp4_read_.close();
}

void Mp4Data::ReadHeaders(std::ifstream& mp4_read_) {
	Atom        atom{};
	BoxHeader   header{};
	std::string type{};

	std::streampos cur = mp4_read_.tellg();
	mp4_read_.seekg(0, std::ios::end);
	std::streampos end = mp4_read_.tellg();
	
	while (cur < end) {	
		mp4_read_.seekg(cur, std::ios::beg);
		mp4_read_.read(reinterpret_cast<char*>(&header), sizeof(header));
		
		header.size = ChangeEndianUInt32(header.size);
		atom = {header, cur};
		
		type = {reinterpret_cast<char*>(header.type), 4};
		mp4_headers_[type].emplace_back(atom);

		if (Mp4Data::IsContainer(type)) {
			cur += (type == "meta") ? 
				static_cast<std::streamoff>(12) :
				static_cast<std::streamoff>(8);
		} else {
			cur += static_cast<std::streamoff>(header.size);
		}
	}
}

void Mp4Data::ReadFtyp(std::ifstream& mp4_read_) {
	const Atom      ftyp_atom        = mp4_headers_["ftyp"].back();
	const BoxHeader head             = ftyp_atom.header;
	const size_t    size             = head.size;
	const size_t    comp_brands_size = size - 16;

	ftyp_ = std::make_unique<Ftyp>();
	ftyp_->comp_brands = std::make_unique<byte[]>(comp_brands_size);

	mp4_read_.seekg(ftyp_atom.offset, std::ios::beg);
	mp4_read_.read(reinterpret_cast<char*>(ftyp_.get()), size - comp_brands_size);
	mp4_read_.read(reinterpret_cast<char*>(ftyp_.get()->comp_brands.get()), comp_brands_size);
	
	mp4_headers_.erase("ftyp");
}

void Mp4Data::ReadMvhd(std::ifstream& mp4_read_) {
	const Atom mvhd_atom = mp4_headers_["mvhd"].back();
	const BoxHeader head = mvhd_atom.header;
	const size_t    size = static_cast<size_t>(head.size);

	mp4_read_.seekg(mvhd_atom.offset, std::ios::beg);

	if (size > sizeof(MvhdBoxVerZero)) {
		mvhd_one_ = std::make_unique<MvhdBoxVerOne>();
		mp4_read_.read(reinterpret_cast<char*>(mvhd_one_.get()), size);
	} else {
		mvhd_zero_ = std::make_unique<MvhdBoxVerZero>();
		mp4_read_.read(reinterpret_cast<char*>(mvhd_zero_.get()), size);
	}
	mp4_headers_.erase("mvhd");
}

void Mp4Data::ReadTkhd(std::ifstream& mp4_read_) {
	for (const auto& tkhd_atom : mp4_headers_["tkhd"]) {
		const BoxHeader head = tkhd_atom.header;
		const size_t    size = static_cast<size_t>(head.size);

		mp4_read_.seekg(tkhd_atom.offset, std::ios::beg);

		if (size > sizeof(TkhdBoxVerZero)) {
			tkhd_one_.emplace_back();
			mp4_read_.read(reinterpret_cast<char*>(&tkhd_one_.back()), size);
		} else {
			tkhd_zero_.emplace_back();
			mp4_read_.read(reinterpret_cast<char*>(&tkhd_zero_.back()), size);
		}
	}
	mp4_headers_.erase("tkhd");
}

void Mp4Data::ReadMdhd(std::ifstream& mp4_read_) {
	for (const auto& mdhd_atom : mp4_headers_["mdhd"]) {
		const BoxHeader head = mdhd_atom.header;
		const size_t    size = static_cast<size_t>(head.size);

		mp4_read_.seekg(mdhd_atom.offset, std::ios::beg);

		if (size > sizeof(MdhdBoxVerZero)) {
			mdhd_one_.emplace_back();
			mp4_read_.read(reinterpret_cast<char*>(&mdhd_one_.back()), size);
		}
		else {
			mdhd_zero_.emplace_back();
			mp4_read_.read(reinterpret_cast<char*>(&mdhd_zero_.back()), size);
		}
	}
	mp4_headers_.erase("mdhd");
}

void Mp4Data::ReadHdlr(std::ifstream& mp4_read_) {
	for (const auto& hdlr_atom : mp4_headers_["hdlr"]) {
		const BoxHeader head   = hdlr_atom.header;
		const size_t    size   = static_cast<size_t>(head.size);
		const size_t name_size = size - 32;

		mp4_read_.seekg(hdlr_atom.offset, std::ios::beg);

		hdlr_.emplace_back();
		hdlr_.back().name = std::make_unique<char[]>(name_size);

		mp4_read_.read(reinterpret_cast<char*>(&hdlr_.back()), size - name_size);
		mp4_read_.read(reinterpret_cast<char*>(hdlr_.back().name.get()), name_size);
	}
	mp4_headers_.erase("hdlr");
}

void Mp4Data::ReadVmhd(std::ifstream& mp4_read_) {
	for (const auto& vmhd_atom : mp4_headers_["vmhd"]) {
		const BoxHeader head = vmhd_atom.header;
		const size_t    size = static_cast<size_t>(head.size);

		mp4_read_.seekg(vmhd_atom.offset, std::ios::beg);
					
		vmhd_.emplace_back();
		mp4_read_.read(reinterpret_cast<char*>(&vmhd_.back()), size);
	}
	mp4_headers_.erase("vmhd");
}

void Mp4Data::ReadSmhd(std::ifstream& mp4_read_) {
	for (const auto& smhd_atom : mp4_headers_["smhd"]) {
		const BoxHeader head = smhd_atom.header;
		const size_t    size = static_cast<size_t>(head.size);

		mp4_read_.seekg(smhd_atom.offset, std::ios::beg);

		smhd_.emplace_back();
		mp4_read_.read(reinterpret_cast<char*>(&smhd_.back()), size);
	}
	mp4_headers_.erase("smhd");
}

void Mp4Data::ReadDref(std::ifstream& mp4_read_) {
	for (const auto& dref_atom : mp4_headers_["dref"]) {
		const BoxHeader head = dref_atom.header;
		const size_t    size = static_cast<size_t>(head.size);
		const size_t url_size = size - 28;

		mp4_read_.seekg(dref_atom.offset, std::ios::beg);

		dref_.emplace_back();
		mp4_read_.read(reinterpret_cast<char*>(&dref_.back()), size - url_size);
		
		if (url_size > 0) {
			dref_.back().url = std::make_unique<char[]>(url_size);
			mp4_read_.read(reinterpret_cast<char*>(dref_.back().url.get()),
				url_size);
		}
	}
	mp4_headers_.erase("dref");
}

void Mp4Data::ReadStsz(std::ifstream& mp4_read_) {
	for (const auto& stsz_atom : mp4_headers_["stsz"]) {
		const BoxHeader head = stsz_atom.header;
		const size_t    size = static_cast<size_t>(head.size);

		mp4_read_.seekg(stsz_atom.offset, std::ios::beg);

		stsz_.emplace_back();
		mp4_read_.read(reinterpret_cast<char*>(&stsz_.back()), 20);

		fbyte sample_size  = ChangeEndianUInt32(stsz_.back().sample_size);
		fbyte sample_count = ChangeEndianUInt32(stsz_.back().sample_count);

		if (sample_size == 0 && sample_count > 0) {
			stsz_.back().ent = std::make_unique<fbyte[]>(sample_count);
			size_t sample_count_size = sample_count * 4;

			mp4_read_.read(reinterpret_cast<char*>(stsz_.back().ent.get()),
				sample_count_size);
		}
	}
	mp4_headers_.erase("stsz");
}

void Mp4Data::ReadStts(std::ifstream& mp4_read_) {
	for (const auto& stts_atom : mp4_headers_["stts"]) {
		const BoxHeader head = stts_atom.header;
		const size_t    size = static_cast<size_t>(head.size);
		const size_t entry_size = size - 16;

		mp4_read_.seekg(stts_atom.offset, std::ios::beg);

		stts_.emplace_back();
		mp4_read_.read(reinterpret_cast<char*>(&stts_.back()), 16);

		size_t entry_count = ChangeEndianUInt32(stts_.back().entry_count);

		stts_.back().entries =
			std::make_unique<SttsEntry[]>(entry_count);
			
		mp4_read_.read(reinterpret_cast<char*>(stts_.back().entries.get()),
			entry_size);
	}
	mp4_headers_.erase("stts");
}

void Mp4Data::AddHeaderFRAK(std::ofstream& mp4_write_, const FrakAtom& frak) {
	const Atom   moov = mp4_headers_["moov"].back();
	const fbyte  moov_size = ChangeEndianUInt32(moov.header.size);
	const fbyte  frak_size = ChangeEndianUInt32(frak.head.size);
	const fbyte  new_moov_size = moov_size + frak_size;
	const fbyte  new_udta_size = new_moov_size - 8;
	const fbyte  hdlr_size     = 33u;
	const fbyte  new_meta_size = new_udta_size - hdlr_size - 8;
	const size_t moov_beg			 = static_cast<size_t>(moov.offset);
	const size_t moov_end      = moov_beg + moov_size;

	const fbyte big_endian_moov_size = ChangeEndianUInt32(new_moov_size);
	mp4_write_.seekp(moov_beg, std::ios::beg);
	mp4_write_.write(
		reinterpret_cast<const char*>(&big_endian_moov_size),
		sizeof(big_endian_moov_size));
	mp4_write_.seekp(moov_end, std::ios::beg);

	if (!mp4_headers_.contains("udta")) {
		const BoxHeader    udta{ChangeEndianUInt32(new_udta_size), {'u','d','t','a'}};
		const HdlrBoxFixed hdlr{
			{ChangeEndianUInt32(hdlr_size), {'h','d','l','r'}},
			(byte)0,
			{'\0','\0','\0'},
			0u,
			{'m','d','t','a'},
			{0u,0u,0u},
			std::make_unique<char[]>(1)};
		const MetaHeader   meta{
			{ChangeEndianUInt32(new_meta_size), {'m','e','t','a'}},
			0u};

		mp4_write_.write(reinterpret_cast<const char*>(&udta), sizeof(udta));
		mp4_write_.write(reinterpret_cast<const char*>(&hdlr), 32);
		mp4_write_.write(reinterpret_cast<const char*>(hdlr.name.get()), 1);
		mp4_write_.write(reinterpret_cast<const char*>(&meta), sizeof(meta));
	} else {
		const size_t udta_beg = moov_beg + 8;
		const size_t meta_beg =
			static_cast<size_t>(mp4_headers_["meta"].back().offset);

		const fbyte big_endian_meta_size = ChangeEndianUInt32(new_meta_size);
		const fbyte big_endian_udta_size = ChangeEndianUInt32(new_udta_size);
		mp4_write_.seekp(udta_beg, std::ios::beg);
		mp4_write_.write(
			reinterpret_cast<const char*>(&big_endian_udta_size), 
			sizeof(big_endian_udta_size));
		mp4_write_.seekp(meta_beg, std::ios::beg);
		mp4_write_.write(
			reinterpret_cast<const char*>(&big_endian_meta_size),
			sizeof(big_endian_meta_size));
	}

	mp4_write_.write(reinterpret_cast<const char*>(&frak), frak_size);
}


std::pair<fbyte, fbyte> Mp4Data::GetWH() {
	size_t i = 0;

	for (const auto& tkhd: tkhd_one_) {
		std::string type(hdlr_[i].handler_type, 4);
		if (type == "vide") {
			fbyte width  = ChangeEndianUInt32(tkhd.width) >> 16;
			fbyte height = ChangeEndianUInt32(tkhd.height) >> 16;
			return { width, height };
		}
		++i;
	}

	for (const auto& tkhd : tkhd_zero_) {
		std::string type(hdlr_[i].handler_type, 4);
		if (type == "vide") {
			fbyte width  = ChangeEndianUInt32(tkhd.width) >> 16;
			fbyte height = ChangeEndianUInt32(tkhd.height) >> 16;
			return {width, height};
		}
		++i;
	}
	return {0u, 0u};
}

size_t Mp4Data::GetVideoDuration() {
	size_t i = 0;

	for (const auto& mdhd : mdhd_one_) {
		std::string type(hdlr_[i].handler_type, 4);
		if (type == "vide") {
			fbyte part_1 = ChangeEndianUInt32(mdhd.duration[0]);
			fbyte part_2 = ChangeEndianUInt32(mdhd.duration[1]);
			return (part_1 << 16) | part_2;
		}
	}

	for (const auto& mdhd : mdhd_zero_) {
		std::string type(hdlr_[i].handler_type, 4);
		if (type == "vide") {
			fbyte part_1 = ChangeEndianUInt32(mdhd.duration);
			return static_cast<size_t>(part_1);
		}
	}
	return 0ull;
}

fbyte Mp4Data::GetTimescale() {
	size_t i = 0;

	for (const auto& mdhd : mdhd_one_) {
		std::string type(hdlr_[i].handler_type, 4);
		if (type == "vide") {
			return ChangeEndianUInt32(mdhd.timescale);
		}
	}

	for (const auto& mdhd : mdhd_zero_) {
		std::string type(hdlr_[i].handler_type, 4);
		if (type == "vide") {
			return ChangeEndianUInt32(mdhd.timescale);
		}
	}
	return 0u;
}

fbyte Mp4Data::GetSampleCount() {
	size_t i = 0;

	for (const auto& stts : stts_) {
		std::string type(hdlr_[i].handler_type, 4);
		if (type == "vide") {
			return ChangeEndianUInt32(stts.entry_count);
		}
	}
	return 0u;
}

fbyte Mp4Data::GetFPS() {	
	size_t i = 0;

	for (const auto& stts : stts_) {
		std::string type(hdlr_[i].handler_type, 4);
		if (type == "vide") {
			fbyte timescale    = GetTimescale();

			if (GetSampleCount() == 1) {
				fbyte sample_delta = ChangeEndianUInt32(stts.entries.get()->sample_delta); 

				return timescale / sample_delta;
			} else {
				size_t total_sample_count   = 0;
				size_t total_duration_ticks = 0;

				for (size_t j = 0, k = GetSampleCount(); j < k; ++j) {
					fbyte sample_entry = ChangeEndianUInt32(stts.entries[j].sample_count);
					fbyte sample_delta = ChangeEndianUInt32(stts.entries[j].sample_delta);

					total_sample_count   += sample_entry;
					total_duration_ticks += (sample_entry * sample_delta);
				}
				double avg_delta_ticks =
					static_cast<double>(total_duration_ticks) / total_sample_count;

				return static_cast<fbyte>(timescale / avg_delta_ticks);
			}
		}
	}
	return 0u;
}

bool Mp4Data::IsContainer(const std::string& type) {
	return kContainerBoxes.contains(type);
}