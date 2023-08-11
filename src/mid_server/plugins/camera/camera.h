
#pragma once

#include <array>
#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace mid {

/**
 * @brief Can be used to manage cameras that implement the MAVLink
 * Camera Protocol: https://mavlink.io/en/protocol/camera.html.
 *
 * Currently only a single camera is supported.
 * When multiple cameras are supported the plugin will need to be
 * instantiated separately for every camera and the camera selected using
 * `select_camera`.
 */
class Camera {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto camera = Camera(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Camera();

    /**
     * @brief Destructor (internal use only).
     */
    virtual ~Camera();

    /**
     * @brief Camera mode type.
     */
    enum class Mode {
        Unknown, /**< @brief Unknown. */
        Photo,   /**< @brief Photo mode. */
        Video,   /**< @brief Video mode. */
    };

    /**
     * @brief Stream operator to print information about a `Camera::Mode`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream &operator<<(std::ostream &str, Camera::Mode const &mode);

    /**
     * @brief Photos range type.
     */
    enum class PhotosRange {
        All,             /**< @brief All the photos present on the camera. */
        SinceConnection, /**< @brief Photos taken since MAVSDK got connected. */
    };

    /**
     * @brief Stream operator to print information about a `Camera::PhotosRange`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream &operator<<(std::ostream &str, Camera::PhotosRange const &photos_range);

    /**
     * @brief Possible results returned for camera commands
     */
    enum class Result {
        Unknown,             /**< @brief Unknown result. */
        Success,             /**< @brief Command executed successfully. */
        InProgress,          /**< @brief Command in progress. */
        Busy,                /**< @brief Camera is busy and rejected command. */
        Denied,              /**< @brief Camera denied the command. */
        Error,               /**< @brief An error has occurred while executing the command. */
        Timeout,             /**< @brief Command timed out. */
        WrongArgument,       /**< @brief Command has wrong argument(s). */
        NoSystem,            /**< @brief No system connected. */
        ProtocolUnsupported, /**< @brief Definition file protocol not supported. */
    };

    /**
     * @brief Stream operator to print information about a `Camera::Result`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream &operator<<(std::ostream &str, Camera::Result const &result);

    /**
     * @brief Position type in global coordinates.
     */
    struct Position {
        double latitude_deg{};       /**< @brief Latitude in degrees (range: -90 to +90) */
        double longitude_deg{};      /**< @brief Longitude in degrees (range: -180 to +180) */
        float absolute_altitude_m{}; /**< @brief Altitude AMSL (above mean sea level) in metres */
        float relative_altitude_m{}; /**< @brief Altitude relative to takeoff altitude in metres */
    };

    /**
     * @brief Equal operator to compare two `Camera::Position` objects.
     *
     * @return `true` if items are equal.
     */
    friend bool operator==(const Camera::Position &lhs, const Camera::Position &rhs);

    /**
     * @brief Stream operator to print information about a `Camera::Position`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream &operator<<(std::ostream &str, Camera::Position const &position);

    /**
     * @brief Quaternion type.
     *
     * All rotations and axis systems follow the right-hand rule.
     * The Hamilton quaternion product definition is used.
     * A zero-rotation quaternion is represented by (1,0,0,0).
     * The quaternion could also be written as w + xi + yj + zk.
     *
     * For more info see: https://en.wikipedia.org/wiki/Quaternion
     */
    struct Quaternion {
        float w{}; /**< @brief Quaternion entry 0, also denoted as a */
        float x{}; /**< @brief Quaternion entry 1, also denoted as b */
        float y{}; /**< @brief Quaternion entry 2, also denoted as c */
        float z{}; /**< @brief Quaternion entry 3, also denoted as d */
    };

    /**
     * @brief Equal operator to compare two `Camera::Quaternion` objects.
     *
     * @return `true` if items are equal.
     */
    friend bool operator==(const Camera::Quaternion &lhs, const Camera::Quaternion &rhs);

    /**
     * @brief Stream operator to print information about a `Camera::Quaternion`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream &operator<<(std::ostream &str, Camera::Quaternion const &quaternion);

    /**
     * @brief Euler angle type.
     *
     * All rotations and axis systems follow the right-hand rule.
     * The Euler angles follow the convention of a 3-2-1 intrinsic Tait-Bryan rotation sequence.
     *
     * For more info see https://en.wikipedia.org/wiki/Euler_angles
     */
    struct EulerAngle {
        float roll_deg{};  /**< @brief Roll angle in degrees, positive is banking to the right */
        float pitch_deg{}; /**< @brief Pitch angle in degrees, positive is pitching nose up */
        float yaw_deg{}; /**< @brief Yaw angle in degrees, positive is clock-wise seen from above */
    };

    /**
     * @brief Equal operator to compare two `Camera::EulerAngle` objects.
     *
     * @return `true` if items are equal.
     */
    friend bool operator==(const Camera::EulerAngle &lhs, const Camera::EulerAngle &rhs);

    /**
     * @brief Stream operator to print information about a `Camera::EulerAngle`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream &operator<<(std::ostream &str, Camera::EulerAngle const &euler_angle);

    /**
     * @brief Information about a picture just captured.
     */
    struct CaptureInfo {
        Position position{};               /**< @brief Location where the picture was taken */
        Quaternion attitude_quaternion{};  /**< @brief Attitude of the camera when the picture was
                                             taken (quaternion) */
        EulerAngle attitude_euler_angle{}; /**< @brief Attitude of the camera when the picture was
                                              taken (euler angle) */
        uint64_t time_utc_us{}; /**< @brief Timestamp in UTC (since UNIX epoch) in microseconds */
        bool is_success{};      /**< @brief True if the capture was successful */
        int32_t index{}; /**< @brief Zero-based index of this image since vehicle was armed */
        std::string file_url{}; /**< @brief Download URL of this image */
    };

    /**
     * @brief Equal operator to compare two `Camera::CaptureInfo` objects.
     *
     * @return `true` if items are equal.
     */
    friend bool operator==(const Camera::CaptureInfo &lhs, const Camera::CaptureInfo &rhs);

    /**
     * @brief Stream operator to print information about a `Camera::CaptureInfo`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream &operator<<(std::ostream &str, Camera::CaptureInfo const &capture_info);

    /**
     * @brief Type for video stream settings.
     */
    struct VideoStreamSettings {
        float frame_rate_hz{};                /**< @brief Frames per second */
        uint32_t horizontal_resolution_pix{}; /**< @brief Horizontal resolution (in pixels) */
        uint32_t vertical_resolution_pix{};   /**< @brief Vertical resolution (in pixels) */
        uint32_t bit_rate_b_s{};              /**< @brief Bit rate (in bits per second) */
        uint32_t rotation_deg{};    /**< @brief Video image rotation (clockwise, 0-359 degrees) */
        std::string uri{};          /**< @brief Video stream URI */
        float horizontal_fov_deg{}; /**< @brief Horizontal fov in degrees */
    };

    /**
     * @brief Equal operator to compare two `Camera::VideoStreamSettings` objects.
     *
     * @return `true` if items are equal.
     */
    friend bool operator==(const Camera::VideoStreamSettings &lhs,
                           const Camera::VideoStreamSettings &rhs);

    /**
     * @brief Stream operator to print information about a `Camera::VideoStreamSettings`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream &operator<<(std::ostream &str,
                                    Camera::VideoStreamSettings const &video_stream_settings);

    /**
     * @brief Information about the video stream.
     */
    struct VideoStreamInfo {
        /**
         * @brief Video stream status type.
         */
        enum class VideoStreamStatus {
            NotRunning, /**< @brief Video stream is not running. */
            InProgress, /**< @brief Video stream is running. */
        };

        /**
         * @brief Stream operator to print information about a `Camera::VideoStreamStatus`.
         *
         * @return A reference to the stream.
         */
        friend std::ostream &operator<<(
            std::ostream &str,
            Camera::VideoStreamInfo::VideoStreamStatus const &video_stream_status);

        /**
         * @brief Video stream light spectrum type
         */
        enum class VideoStreamSpectrum {
            Unknown,      /**< @brief Unknown. */
            VisibleLight, /**< @brief Visible light. */
            Infrared,     /**< @brief Infrared. */
        };

        /**
         * @brief Stream operator to print information about a `Camera::VideoStreamSpectrum`.
         *
         * @return A reference to the stream.
         */
        friend std::ostream &operator<<(
            std::ostream &str,
            Camera::VideoStreamInfo::VideoStreamSpectrum const &video_stream_spectrum);

        int32_t stream_id{};            /**< @brief stream unique id */
        VideoStreamSettings settings{}; /**< @brief Video stream settings */
        VideoStreamStatus status{};     /**< @brief Current status of video streaming */
        VideoStreamSpectrum spectrum{}; /**< @brief Light-spectrum of the video stream */
    };

    /**
     * @brief Equal operator to compare two `Camera::VideoStreamInfo` objects.
     *
     * @return `true` if items are equal.
     */
    friend bool operator==(const Camera::VideoStreamInfo &lhs, const Camera::VideoStreamInfo &rhs);

    /**
     * @brief Stream operator to print information about a `Camera::VideoStreamInfo`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream &operator<<(std::ostream &str,
                                    Camera::VideoStreamInfo const &video_stream_info);

    /**
     * @brief Information about the camera status.
     */
    struct Status {
        /**
         * @brief Storage status type.
         */
        enum class StorageStatus {
            NotAvailable, /**< @brief Status not available. */
            Unformatted,  /**< @brief Storage is not formatted (i.e. has no recognized file system).
                          */
            Formatted,    /**< @brief Storage is formatted (i.e. has recognized a file system). */
            NotSupported, /**< @brief Storage status is not supported. */
        };

        /**
         * @brief Stream operator to print information about a `Camera::StorageStatus`.
         *
         * @return A reference to the stream.
         */
        friend std::ostream &operator<<(std::ostream &str,
                                        Camera::Status::StorageStatus const &storage_status);

        /**
         * @brief Storage type.
         */
        enum class StorageType {
            Unknown,  /**< @brief Storage type unknown. */
            UsbStick, /**< @brief Storage type USB stick. */
            Sd,       /**< @brief Storage type SD card. */
            Microsd,  /**< @brief Storage type MicroSD card. */
            Hd,       /**< @brief Storage type HD mass storage. */
            Other,    /**< @brief Storage type other, not listed. */
        };

        /**
         * @brief Stream operator to print information about a `Camera::StorageType`.
         *
         * @return A reference to the stream.
         */
        friend std::ostream &operator<<(std::ostream &str,
                                        Camera::Status::StorageType const &storage_type);

        bool video_on{};          /**< @brief Whether video recording is currently in process */
        bool photo_interval_on{}; /**< @brief Whether a photo interval is currently in process */
        float used_storage_mib{}; /**< @brief Used storage (in MiB) */
        float available_storage_mib{}; /**< @brief Available storage (in MiB) */
        float total_storage_mib{};     /**< @brief Total storage (in MiB) */
        float recording_time_s{}; /**< @brief Elapsed time since starting the video recording (in
                                     seconds) */
        std::string media_folder_name{}; /**< @brief Current folder name where media are saved */
        StorageStatus storage_status{};  /**< @brief Storage status */
        uint32_t storage_id{};           /**< @brief Storage ID starting at 1 */
        StorageType storage_type{};      /**< @brief Storage type */
    };

    /**
     * @brief Equal operator to compare two `Camera::Status` objects.
     *
     * @return `true` if items are equal.
     */
    friend bool operator==(const Camera::Status &lhs, const Camera::Status &rhs);

    /**
     * @brief Stream operator to print information about a `Camera::Status`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream &operator<<(std::ostream &str, Camera::Status const &status);

    /**
     * @brief Type to represent a setting option.
     */
    struct Option {
        std::string option_id{};          /**< @brief Name of the option (machine readable) */
        std::string option_description{}; /**< @brief Description of the option (human readable) */
    };

    /**
     * @brief Equal operator to compare two `Camera::Option` objects.
     *
     * @return `true` if items are equal.
     */
    friend bool operator==(const Camera::Option &lhs, const Camera::Option &rhs);

    /**
     * @brief Stream operator to print information about a `Camera::Option`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream &operator<<(std::ostream &str, Camera::Option const &option);

    /**
     * @brief Type to represent a setting with a selected option.
     */
    struct Setting {
        std::string setting_id{};          /**< @brief Name of a setting (machine readable) */
        std::string setting_description{}; /**< @brief Description of the setting (human readable).
                                              This field is meant to be read from the drone, ignore
                                              it when setting. */
        Option option{};                   /**< @brief Selected option */
        bool is_range{}; /**< @brief If option is given as a range. This field is meant to be read
                            from the drone, ignore it when setting. */
    };

    /**
     * @brief Equal operator to compare two `Camera::Setting` objects.
     *
     * @return `true` if items are equal.
     */
    friend bool operator==(const Camera::Setting &lhs, const Camera::Setting &rhs);

    /**
     * @brief Stream operator to print information about a `Camera::Setting`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream &operator<<(std::ostream &str, Camera::Setting const &setting);

    /**
     * @brief Type to represent a setting with a list of options to choose from.
     */
    struct SettingOptions {
        std::string setting_id{}; /**< @brief Name of the setting (machine readable) */
        std::string
            setting_description{}; /**< @brief Description of the setting (human readable) */
        std::vector<Option>
            options{}; /**< @brief List of options or if range [min, max] or [min, max, interval] */
        bool is_range{}; /**< @brief If option is given as a range */
    };

    /**
     * @brief Equal operator to compare two `Camera::SettingOptions` objects.
     *
     * @return `true` if items are equal.
     */
    friend bool operator==(const Camera::SettingOptions &lhs, const Camera::SettingOptions &rhs);

    /**
     * @brief Stream operator to print information about a `Camera::SettingOptions`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream &operator<<(std::ostream &str,
                                    Camera::SettingOptions const &setting_options);

    /**
     * @brief Type to represent a camera information.
     */
    struct Information {
        /**
         * @brief
         */
        enum class CameraCapFlags {
            CaptureVideo,               /**< @brief Camera is able to record video. */
            CaptureImage,               /**< @brief Camera is able to capture images. */
            HasModes,                   /**< @brief Camera has separate Video and Image/Photo modes
                         (MAV_CMD_SET_CAMERA_MODE). */
            CanCaptureImageInVideoMode, /**< @brief Camera can capture images while in video mode.
                                         */
            CanCaptureVideoInImageMode, /**< @brief Camera can capture videos while in Photo/Image
                                           mode. */
            HasImageSurveyMode, /**< @brief Camera has image survey mode (MAV_CMD_SET_CAMERA_MODE).
                                 */
            HasBasicZoom,  /**< @brief Camera has basic zoom control (MAV_CMD_SET_CAMERA_ZOOM). */
            HasBasicFocus, /**< @brief Camera has basic focus control (MAV_CMD_SET_CAMERA_FOCUS). */
            HasVideoStream,   /**< @brief Camera has video streaming capabilities (request
                               VIDEO_STREAM_INFORMATION with MAV_CMD_REQUEST_MESSAGE for video
                               streaming info). */
            HasTrackingPoint, /**< @brief Camera supports tracking of a point on the camera view..
                               */
            HasTrackingRectangle, /**< @brief Camera supports tracking of a selection rectangle on
                                     the camera view.. */
            HasTrackingGeoStatus, /**< @brief Camera supports tracking geo status
                                     (CAMERA_TRACKING_GEO_STATUS).. */
        };

        /**
         * @brief Stream operator to print information about a `Camera::CameraCapFlags`.
         *
         * @return A reference to the stream.
         */
        friend std::ostream &operator<<(
            std::ostream &str, Camera::Information::CameraCapFlags const &camera_cap_flags);

        std::string vendor_name{};           /**< @brief Name of the camera vendor */
        std::string model_name{};            /**< @brief Name of the camera model */
        std::string firmware_version{};      /**< @brief Camera firmware version in
                                           major[.minor[.patch[.dev]]] format */
        float focal_length_mm{};             /**< @brief Focal length */
        float horizontal_sensor_size_mm{};   /**< @brief Horizontal sensor size */
        float vertical_sensor_size_mm{};     /**< @brief Vertical sensor size */
        uint32_t horizontal_resolution_px{}; /**< @brief Horizontal image resolution in pixels */
        uint32_t vertical_resolution_px{};   /**< @brief Vertical image resolution in pixels */
        uint32_t lens_id{};                  /**< @brief Lens ID */
        uint32_t
            definition_file_version{}; /**< @brief Camera definition file version (iteration) */
        std::string
            definition_file_uri{}; /**< @brief Camera definition URI (http or mavlink ftp) */
        std::vector<CameraCapFlags>
            camera_cap_flags{}; /**< @brief Camera capability flags (Array) */
    };

    /**
     * @brief Equal operator to compare two `Camera::Information` objects.
     *
     * @return `true` if items are equal.
     */
    friend bool operator==(const Camera::Information &lhs, const Camera::Information &rhs);

    /**
     * @brief Stream operator to print information about a `Camera::Information`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream &operator<<(std::ostream &str, Camera::Information const &information);

    /**
     * @brief Prepare the camera plugin (e.g. download the camera definition, etc).
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    virtual Result prepare() const = 0;

    /**
     * @brief Take one photo.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    virtual Result take_photo() const = 0;

    /**
     * @brief Start photo timelapse with a given interval.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    virtual Result start_photo_interval(float interval_s) const = 0;

    /**
     * @brief Stop a running photo timelapse.
     *
     * This function is blocking. See 'stop_photo_interval_async' for the non-blocking counterpart.
     *
     * @return Result of request.
     */
    virtual Result stop_photo_interval() const = 0;

    /**
     * @brief Start a video recording.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    virtual Result start_video() const = 0;

    /**
     * @brief Stop a running video recording.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    virtual Result stop_video() const = 0;

    /**
     * @brief Start video streaming.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    virtual Result start_video_streaming(int32_t stream_id) const = 0;

    /**
     * @brief Stop current video streaming.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    virtual Result stop_video_streaming(int32_t stream_id) const = 0;

    /**
     * @brief Set camera mode.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    virtual Result set_mode(Mode mode) const = 0;

    /**
     * @brief List photos available on the camera.
     *
     * This function is blocking. See 'list_photos_async' for the non-blocking counterpart.
     *
     * @return Result of request.
     */
    virtual std::pair<Result, std::vector<Camera::CaptureInfo>> list_photos(
        PhotosRange photos_range) const = 0;

    /**
     * @brief Callback type for subscribe_mode.
     */
    using ModeCallback = std::function<void(Mode)>;

    /**
     * @brief Subscribe to camera mode updates.
     */
    virtual void subscribe_mode(const ModeCallback &callback) = 0;

    /**
     * @brief Poll for 'Mode' (blocking).
     *
     * @return One Mode update.
     */
    virtual Mode mode() const = 0;

    /**
     * @brief Callback type for subscribe_information.
     */
    using InformationCallback = std::function<void(Information)>;

    /**
     * @brief Subscribe to camera information updates.
     */
    virtual void subscribe_information(const InformationCallback &callback) = 0;

    /**
     * @brief Poll for 'Information' (blocking).
     *
     * @return One Information update.
     */
    virtual Information information() const = 0;

    /**
     * @brief Callback type for subscribe_video_stream_info.
     */
    using VideoStreamInfoCallback = std::function<void(std::vector<VideoStreamInfo>)>;

    /**
     * @brief Subscribe to video stream info updates.
     */
    virtual void subscribe_video_stream_info(const VideoStreamInfoCallback &callback) = 0;

    /**
     * @brief Poll for 'VideoStreamInfo' (blocking).
     *
     * @return One std::vector<VideoStreamInfo> update.
     */
    virtual std::vector<VideoStreamInfo> video_stream_info() const = 0;

    /**
     * @brief Callback type for subscribe_capture_info.
     */
    using CaptureInfoCallback = std::function<void(CaptureInfo)>;

    /**
     * @brief Subscribe to capture info updates.
     */
    virtual void subscribe_capture_info(const CaptureInfoCallback &callback) = 0;

    /**
     * @brief Callback type for subscribe_status.
     */
    using StatusCallback = std::function<void(Status)>;

    /**
     * @brief Subscribe to camera status updates.
     */
    virtual void subscribe_status(const StatusCallback &callback) = 0;

    /**
     * @brief Poll for 'Status' (blocking).
     *
     * @return One Status update.
     */
    virtual Status status() const = 0;

    /**
     * @brief Callback type for subscribe_current_settings.
     */
    using CurrentSettingsCallback = std::function<void(std::vector<Setting>)>;

    /**
     * @brief Get the list of current camera settings.
     */
    virtual void subscribe_current_settings(const CurrentSettingsCallback &callback) = 0;

    /**
     * @brief Callback type for subscribe_possible_setting_options.
     */
    using PossibleSettingOptionsCallback = std::function<void(std::vector<SettingOptions>)>;

    /**
     * @brief Get the list of settings that can be changed.
     */
    virtual void subscribe_possible_setting_options(
        const PossibleSettingOptionsCallback &callback) = 0;

    /**
     * @brief Poll for 'std::vector<SettingOptions>' (blocking).
     *
     * @return One std::vector<SettingOptions> update.
     */
    virtual std::vector<SettingOptions> possible_setting_options() const = 0;

    /**
     * @brief Set a setting to some value.
     *
     * Only setting_id of setting and option_id of option needs to be set.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    virtual Result set_setting(Setting setting) const = 0;

    /**
     * @brief Callback type for get_setting_async.
     */
    using GetSettingCallback = std::function<void(Result, Setting)>;

    /**
     * @brief Get a setting.
     *
     * Only setting_id of setting needs to be set.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    virtual std::pair<Result, Camera::Setting> get_setting(Setting setting) const = 0;

    /**
     * @brief Format storage (e.g. SD card) in camera.
     *
     * This will delete all content of the camera storage!
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    virtual Result format_storage(int32_t storage_id) const = 0;

    /**
     * @brief Select current camera .
     *
     * Bind the plugin instance to a specific camera_id
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    virtual Result select_camera(int32_t camera_id) const = 0;

    /**
     * @brief Reset all settings in camera.
     *
     * This will reset all camera settings to default value
     *
     * This function is blocking. See 'reset_settings_async' for the non-blocking counterpart.
     *
     * @return Result of request.
     */
    virtual Result reset_settings() const = 0;

    /**
     * @brief Manual set the definition data
     *
     * The camera will use the definition data to config the camera
     * The camera already support http protocol to download definition file
     * But we want to support mavlink ftp way to download file too.
     * We don't want the camera to use file system to maintain the definition file.
     * So we use mavlink ftp to download the definition file first,
     * and read the definition file data to manual set.
     *
     * This function is blocking.
     *
     * @return Result of request.
     */
    virtual Result set_definition_data(std::string definition_data) const = 0;

    /**
     * @brief Copy constructor.
     */
    Camera(const Camera &other) = delete;

    /**
     * @brief Equality operator (object is not copyable).
     */
    const Camera &operator=(const Camera &) = delete;
};

}  // namespace mid