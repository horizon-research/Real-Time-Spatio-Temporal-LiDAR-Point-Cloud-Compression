
template <class T>
void export_vectors(std::vector<T>& data, std::string filename) {
  std::ofstream out_stream(filename, std::ofstream::binary);

  for (T d : data) {
    out_stream.write(reinterpret_cast<const char*>(&d), sizeof(d));
  }

  out_stream.close();
}

template <class T>
void import_vectors(std::vector<T>& data, std::string filename) {
  std::ifstream in_stream(filename, std::ifstream::binary);
  T d;
  while (in_stream.read(reinterpret_cast<char*>(&d), sizeof(T))) {
    data.push_back(d);
  }

  in_stream.close();
}

