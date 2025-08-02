#ifndef DIPLOM_SRC_MP4_PARSER_H_
#define DIPLOM_SRC_MP4_PARSER_H_

#include <fstream>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

using byte  = unsigned char;
using fbyte = unsigned int;

namespace mp4parser {

#pragma pack(push, 1)

struct BoxHeader {
	fbyte size;
	byte type[4];
};

struct MyAtom {
	BoxHeader head;

	fbyte duration;
	fbyte timescale;
	fbyte frames_count;
	fbyte width;
	fbyte height;
};

struct Atom {
	BoxHeader header;
	std::streampos offset;
};

struct Ftyp {
	BoxHeader head{};
	byte  major_brand[4]{};
	byte  minor_versn[4]{};
	std::unique_ptr<byte[]> comp_brands = nullptr;
};

struct MvhdBoxVerZero {
	BoxHeader head;

	byte  version;
	byte  flags[3];

	fbyte creation_time;
	fbyte modification_time;
	fbyte timescale;
	fbyte duration;
  fbyte rate;
	byte volume[2];
	byte reserved[10];
	fbyte matrix[9];
	fbyte pre_defined[6];
	fbyte next_track_id;
};

struct MvhdBoxVerOne {
	BoxHeader head;
	
	byte  version;
	byte  flags[3];

	fbyte creation_time[2];
	fbyte modification_time[2];
	fbyte timescale;
	fbyte duration[2];
	fbyte rate;
	byte volume[2];
	byte reserved[10];
	fbyte matrix[9];
	fbyte pre_defined[6];
	fbyte next_track_id;
};

struct TkhdBoxVerZero {
	BoxHeader head;

	byte version;
	byte flags[3];

	fbyte creation_time;
	fbyte modification_time;
	fbyte track_id;
	fbyte reserved1;
	fbyte duration;

	fbyte reserved2[2];
	byte layer[2];
	byte alternate_group[2];
	byte volume[2];
	byte reserved3[2];

	fbyte matrix[9];    
	fbyte width;          
	fbyte height;         
};

struct TkhdBoxVerOne {
	BoxHeader head;

	byte version;
	byte flags[3];

	fbyte creation_time[2];
	fbyte modification_time[2];
	fbyte track_id;
	fbyte reserved1;
	fbyte duration[2];

	fbyte reserved2[2];
	byte layer[2];
	byte alternate_group[2];
	byte volume[2];
	byte reserved3[2];

	fbyte matrix[9];
	fbyte width;
	fbyte height;
};

struct MdhdBoxVerZero {
	BoxHeader head;

	byte  version;
	byte  flags[3];

	fbyte creation_time;
	fbyte modification_time;
	fbyte timescale;   
	fbyte duration;    

	byte language[2];
	byte pre_defined[2];
};	

struct MdhdBoxVerOne {
	BoxHeader head;

	byte  version;
	byte  flags[3];

	fbyte creation_time[2];
	fbyte modification_time[2];
	fbyte timescale;
	fbyte duration[2];

	byte language[2];
	byte pre_defined[2];
};

struct HdlrBoxFixed {
	BoxHeader head;

	byte  version;
	byte  flags[3];
	fbyte pre_defined;
	char  handler_type[4]; 
	fbyte reserved[3];     
	std::unique_ptr<char[]> name = nullptr;
};

struct VmhdBox {
	BoxHeader head;

	byte  version;         
	byte  flags[3];         
	byte graphics_mode[2]; 
	byte opcolor[6];       
};

struct SmhdBox {
	BoxHeader head;

	byte version;       
	byte flags[3];      
	byte balance[2];    
	byte reserved[2];   
};

struct DrefBox {
	BoxHeader head;

	byte version;   
	byte flags[3];      
	fbyte entry_count;   
	
	fbyte size;         
	char type[4];       
	byte url_version;       
	byte url_flags[3];

	std::unique_ptr<char[]> url = nullptr;
};

struct StszBox {
	BoxHeader head;
	byte  version;
	byte  flags[3];
	fbyte sample_size;
	fbyte sample_count;
	std::unique_ptr<fbyte[]> ent = nullptr;
};

struct SttsEntry {
	fbyte sample_count;
	fbyte sample_delta;
};

struct SttsBox {
	BoxHeader head;
	byte version;
	byte flags[3];
	fbyte entry_count;
	std::unique_ptr<SttsEntry[]> entries = nullptr;
};

struct FrakAtom {
	BoxHeader head;
	 
};

struct MetaHeader {
	BoxHeader head;
	fbyte flags;
};

#pragma pack(pop)

class Mp4Data {
public:
	static constexpr int err_path = 1;

	explicit Mp4Data(std::string_view mp4_path);

	unsigned int GetError() const noexcept { return error_; }

  std::pair<fbyte, fbyte> GetWH();

	size_t GetVideoDuration();

	fbyte GetTimescale();

	fbyte GetSampleCount();

	fbyte GetFPS();

protected:

private:
	static inline const std::unordered_set<std::string> kContainerBoxes = {
		"moov", "trak", "mdia", "minf", "stbl",
		"dinf", "mvex", "edts", "udta", "meta", "ilst"
	};

	unsigned int error_;
	
	std::unordered_map<std::string, std::vector<Atom>> mp4_headers_{};

	std::unique_ptr<Ftyp>           ftyp_        = nullptr;

	std::unique_ptr<MvhdBoxVerZero> mvhd_zero_   = nullptr;

	std::unique_ptr<MvhdBoxVerOne>	mvhd_one_    = nullptr;

	std::vector<TkhdBoxVerZero>			tkhd_zero_{};
		
	std::vector<TkhdBoxVerOne>		  tkhd_one_ {};

	std::vector<MdhdBoxVerZero>			mdhd_zero_{};

	std::vector<MdhdBoxVerOne>		  mdhd_one_ {};

	std::vector<HdlrBoxFixed>				hdlr_{};

	std::vector<VmhdBox>						vmhd_{};

	std::vector<SmhdBox>						smhd_{};

	std::vector<DrefBox>						dref_{};

	std::vector<StszBox>						stsz_{};
	
	std::vector<SttsBox>						stts_{};

	
	static bool IsContainer(const std::string& type);

	void ReadHeaders(std::ifstream& mp4_read_);

	void ReadFtyp(std::ifstream& mp4_read_);

	void ReadMvhd(std::ifstream& mp4_read_);

	void ReadTkhd(std::ifstream& mp4_read_);

	void ReadMdhd(std::ifstream& mp4_read_);

	void ReadHdlr(std::ifstream& mp4_read_);

	void ReadVmhd(std::ifstream& mp4_read_);

	void ReadSmhd(std::ifstream& mp4_read_);

	void ReadDref(std::ifstream& mp4_read_);

	void ReadStsz(std::ifstream& mp4_read_);

	void ReadStts(std::ifstream& mp4_read_);

	void AddHeaderFRAK(std::ofstream& mp4_write_, const FrakAtom& frak);
};

}; // mp4parser

#endif // DIPLOM_SRC_MP4_PARSER_H_
