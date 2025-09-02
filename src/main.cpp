
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Microsoft.Windows.AI.h>
#include <winrt/Windows.Data.Json.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <future>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Imaging;
using namespace Windows::Storage::Streams;
using namespace Microsoft::Windows::AI::Imaging;
using namespace Windows::Data::Json;

IAsyncOperation<JsonObject> ProcessImageAsync(std::wstring const& filePath, int index)
{
    std::ifstream file(filePath, std::ios::binary);
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});
    InMemoryRandomAccessStream memStream;
    DataWriter writer(memStream);
    writer.WriteBytes(buffer);
    co_await writer.StoreAsync();
    memStream.Seek(0);

    BitmapDecoder decoder = co_await BitmapDecoder::CreateAsync(memStream);
    SoftwareBitmap bitmap = co_await decoder.GetSoftwareBitmapAsync();
    SoftwareBitmap grayBitmap = SoftwareBitmap::Convert(bitmap, BitmapPixelFormat::Gray8, BitmapAlphaMode::Ignore);

    TextRecognizer recognizer = co_await TextRecognizer::CreateAsync();
    ImageBuffer bufferForOcr = ImageBuffer::CreateForSoftwareBitmap(grayBitmap);
    RecognizedText result = co_await recognizer.RecognizeTextFromImageAsync(bufferForOcr);

    JsonArray linesArray;
    for (auto const& line : result.Lines())
    {
        JsonObject obj;
        obj.SetNamedValue(L"text", JsonValue::CreateStringValue(line.Text()));
        auto rect = line.BoundingRect();
        JsonObject bbox;
        bbox.SetNamedValue(L"x", JsonValue::CreateNumberValue(rect.X));
        bbox.SetNamedValue(L"y", JsonValue::CreateNumberValue(rect.Y));
        bbox.SetNamedValue(L"width", JsonValue::CreateNumberValue(rect.Width));
        bbox.SetNamedValue(L"height", JsonValue::CreateNumberValue(rect.Height));
        obj.SetNamedValue(L"boundingBox", bbox);
        linesArray.Append(obj);
    }

    JsonObject pageObj;
    pageObj.SetNamedValue(L"imageIndex", JsonValue::CreateNumberValue(index));
    pageObj.SetNamedValue(L"lines", linesArray);
    co_return pageObj;
}

int main(int argc, char* argv[])
{
    init_apartment();
    std::vector<std::future<JsonObject>> futures;

    for (int i = 1; i < argc; ++i)
    {
        std::wstring filePath = std::wstring(argv[i], argv[i] + strlen(argv[i]));
        futures.push_back(std::async(std::launch::async, [filePath, i]() {
            return ProcessImageAsync(filePath, i).get();
        }));
    }

    JsonArray allResults;
    for (auto& f : futures)
    {
        allResults.Append(f.get());
    }

    std::wcout << allResults.Stringify().c_str() << std::endl;
    return 0;
}
