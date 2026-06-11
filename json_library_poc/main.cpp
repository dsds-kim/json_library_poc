#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// JSON 값을 재귀적으로 string으로 펼쳐서 unordered_map에 담는다.
// 중첩 키는 "parent.child" 형식으로 flatten.
static void flatten_to_map(
    const json& j,
    std::unordered_map<std::string, std::string>& out,
    const std::string& prefix = "")
{
    if (j.is_object()) {
        for (auto& [key, val] : j.items()) {
            std::string full_key = prefix.empty() ? key : prefix + "." + key;
            flatten_to_map(val, out, full_key);
        }
    } else if (j.is_array()) {
        for (size_t i = 0; i < j.size(); ++i) {
            std::string full_key = prefix + "[" + std::to_string(i) + "]";
            flatten_to_map(j[i], out, full_key);
        }
    } else {
        // 기본 타입: string/number/bool/null 모두 dump()로 string화
        out[prefix] = j.is_string() ? j.get<std::string>() : j.dump();
    }
}

// ── Demo 1: JSON 문자열 파싱 & string으로 직렬화 ──────────────────────────
static void demo_parse_and_serialize()
{
    std::cout << "\n=== Demo 1: 파싱 & 직렬화 ===\n";

    const std::string raw = R"({
        "name": "Alice",
        "age": 30,
        "active": true,
        "score": 98.5
    })";

    json j = json::parse(raw);

    // pretty-print (들여쓰기 4칸)
    std::string pretty = j.dump(4);
    std::cout << "[pretty]\n" << pretty << "\n";

    // compact string
    std::string compact = j.dump();
    std::cout << "[compact] " << compact << "\n";

    // 개별 필드를 string으로
    std::string name   = j.at("name").get<std::string>();
    int         age    = j.at("age").get<int>();
    bool        active = j.at("active").get<bool>();
    double      score  = j.at("score").get<double>();

    std::cout << "name=" << name
              << "  age=" << age
              << "  active=" << std::boolalpha << active
              << "  score=" << score << "\n";
}

// ── Demo 2: JSON 객체 → unordered_map<string, string> ─────────────────────
static void demo_flat_to_map()
{
    std::cout << "\n=== Demo 2: flat object → unordered_map ===\n";

    const std::string raw = R"({
        "host": "localhost",
        "port": "8080",
        "db": "mydb",
        "timeout": "30"
    })";

    json j = json::parse(raw);

    // 타입이 동일하게 string인 단순 설정 맵
    std::unordered_map<std::string, std::string> config;
    for (auto& [key, val] : j.items()) {
        config[key] = val.get<std::string>();
    }

    for (auto& [k, v] : config) {
        std::cout << "  " << k << " = " << v << "\n";
    }
}

// ── Demo 3: 중첩 JSON → flatten unordered_map ─────────────────────────────
static void demo_nested_to_flat_map()
{
    std::cout << "\n=== Demo 3: 중첩 JSON → flatten map ===\n";

    const std::string raw = R"({
        "server": {
            "host": "192.168.1.1",
            "port": 9090
        },
        "auth": {
            "user": "admin",
            "roles": ["read", "write"]
        },
        "version": "1.0.0"
    })";

    json j = json::parse(raw);

    std::unordered_map<std::string, std::string> flat;
    flatten_to_map(j, flat);

    for (auto& [k, v] : flat) {
        std::cout << "  " << k << " = " << v << "\n";
    }
}

// ── Demo 4: 파일에서 JSON 읽기 ────────────────────────────────────────────
static void demo_read_from_file(const std::string& path)
{
    std::cout << "\n=== Demo 4: 파일 읽기 (" << path << ") ===\n";

    std::ifstream fs(path);
    if (!fs.is_open()) {
        std::cout << "  파일을 열 수 없습니다: " << path << "\n";
        return;
    }

    json j = json::parse(fs);

    // 배열 순회
    if (j.is_array()) {
        std::cout << "  항목 수: " << j.size() << "\n";
        for (size_t i = 0; i < j.size(); ++i) {
            std::cout << "  [" << i << "] " << j[i].dump() << "\n";
        }
    } else {
        std::cout << j.dump(2) << "\n";
    }
}

// ── Demo 5: JSON 빌드 & 수정 ──────────────────────────────────────────────
static void demo_build_and_modify()
{
    std::cout << "\n=== Demo 5: JSON 빌드 & 수정 ===\n";

    // 코드로 직접 JSON 구성
    json j;
    j["id"]     = 1;
    j["label"]  = "test";
    j["tags"]   = json::array({"cpp", "json", "poc"});
    j["meta"]["created"] = "2026-06-11";

    std::cout << "[before] " << j.dump(2) << "\n";

    // 필드 수정
    j["label"] = "updated";
    j["tags"].push_back("nlohmann");
    j.erase("meta");

    std::cout << "[after]  " << j.dump(2) << "\n";
}

int main()
{
    try {
        demo_parse_and_serialize();
        demo_flat_to_map();
        demo_nested_to_flat_map();
        demo_read_from_file("sample_data.json");
        demo_build_and_modify();
    } catch (const json::exception& e) {
        std::cerr << "[JSON 오류] " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "[오류] " << e.what() << "\n";
        return 1;
    }

    return 0;
}
