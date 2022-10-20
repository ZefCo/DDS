# include <iostream>
# include <filesystem>

namespace fs = std::filesystem;

int main() {

    fs::path cwd;

    cwd = fs::current_path();

    std::cout << "Current path is " << cwd << std::endl;

    fs::path updir = cwd.parent_path();

    std::cout << "Up one diretory is " << updir << std::endl;

    fs::path image_dir = updir / "LatticeImages";

    std::cout << "The lattice dir is " << image_dir << std::endl;

    if (fs::exists(image_dir)) {std::cout << image_dir << " Already exists" << std::endl;}
    else {fs::create_directory(image_dir);}

}