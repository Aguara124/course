#include "dto2modelmapper.h"
#include "assignment.h"
#include "assignmentdto.h"
#include "assistant.h"
#include "assistantdto.h"
#include "consultation.h"
#include "consultationdto.h"
#include "fcs.h"
#include "fcsdto.h"
#include "grade.h"
#include "gradedto.h"
#include "student.h"
#include "studentdto.h"
#include "teacher.h"
#include "teacherdto.h"
#include "work.h"
#include "workdto.h"

Dto2ModelMapper::Dto2ModelMapper() {}

Assignment *Dto2ModelMapper::convertFromDto(const AssignmentDto &dto)
{
    Assignment *assignmentPtr = new Assignment();

    assignmentPtr->setAssign(QByteArray::fromBase64(dto.getAssign()));
    assignmentPtr->setComment(dto.getComment());
    assignmentPtr->setControl(dto.getComment());
    assignmentPtr->setDeadLine(dto.getDeadLine());
    assignmentPtr->setExaminerName(dto.getExaminerName());
    assignmentPtr->setGradingTable(QByteArray::fromBase64(dto.getGradingTable()));
    assignmentPtr->setGroup(dto.getGroup());
    assignmentPtr->setId(dto.getId());
    assignmentPtr->setIsPdf(dto.getIsPdf());
    assignmentPtr->setName(dto.getName());
    assignmentPtr->setSubGroup(dto.getSubGroup());
    assignmentPtr->setSubject(dto.getSubject());

    return assignmentPtr;
}

AssignmentDto Dto2ModelMapper::convertFromModel(const Assignment &assignment)
{
    AssignmentDto dto;

    dto.setAssign(assignment.getAssign().toBase64());
    dto.setComment(assignment.getComment());
    dto.setControl(assignment.getComment());
    dto.setDeadLine(assignment.getDeadLine());
    dto.setExaminerName(assignment.getExaminerName());
    dto.setGradingTable(assignment.getGradingTable().toBase64());
    dto.setGroup(assignment.getGroup());
    dto.setId(assignment.getId());
    dto.setIsPdf(assignment.getIsPdf());
    dto.setName(assignment.getName());
    dto.setSubGroup(assignment.getSubGroup());
    dto.setSubject(assignment.getSubject());

    return dto;
}

Assistant *Dto2ModelMapper::convertFromDto(const AssistantDto &assistantDto)
{
    Assistant *assistant = new Assistant();

    assistant->setId(assistantDto.getId());
    assistant->setLogin(assistantDto.getLogin());
    assistant->setPassword(assistantDto.getPassword());
    assistant->setProgram(assistantDto.getProgram());
    assistant->setGroup(assistantDto.getGroup());
    assistant->setSubGroup(assistantDto.getSubGroup());
    assistant->setTeacher(assistantDto.getTeacher());
    assistant->setSubject(assistantDto.getSubject());
    assistant->setAvatar(assistantDto.getAvatar());
    assistant->setName(assistantDto.getName());
    assistant->setSurname(assistantDto.getSurname());

    return assistant;
}

AssistantDto Dto2ModelMapper::convertFromModel(const Assistant &assistant)
{
    AssistantDto assistantDto;

    assistantDto.setId(assistant.getId());
    assistantDto.setLogin(assistant.getLogin());
    assistantDto.setPassword(assistant.getPassword());
    assistantDto.setProgram(assistant.getProgram());
    assistantDto.setGroup(assistant.getGroup());
    assistantDto.setSubGroup(assistant.getSubGroup());
    assistantDto.setTeacher(assistant.getTeacher());
    assistantDto.setSubject(assistant.getSubject());
    assistantDto.setAvatar(assistant.getAvatar());
    assistantDto.setName(assistant.getName());
    assistantDto.setSurname(assistant.getSurname());

    return assistantDto;
}

Consultation *Dto2ModelMapper::convertFromDto(const ConsultationDto &consultationDto)
{
    Consultation *consultation = new Consultation();
    consultation->setId(consultationDto.getId());
    consultation->setGroup(consultationDto.getGroup());
    consultation->setSubGroup(consultationDto.getSubGroup());

    consultation->setLogin(consultationDto.getLogin());
    consultation->setName(consultationDto.getName());
    consultation->setSurName(consultationDto.getSurName());
    consultation->setSubject(consultationDto.getSubject());
    consultation->setExaminerName(consultationDto.getExaminerName());
    consultation->setQuestion(consultationDto.getQuestion());
    consultation->setDate(consultationDto.getDate());
    consultation->setTime(consultationDto.getTime());
    consultation->setChecked(consultationDto.getChecked());

    return consultation;
}

ConsultationDto Dto2ModelMapper::convertFromModel(const Consultation &consultation)
{
    ConsultationDto dto;

    dto.setId(consultation.getId());
    dto.setGroup(consultation.getGroup());
    dto.setSubGroup(consultation.getSubGroup());

    dto.setLogin(consultation.getLogin());
    dto.setName(consultation.getName());
    dto.setSurName(consultation.getSurName());
    dto.setSubject(consultation.getSubject());
    dto.setExaminerName(consultation.getExaminerName());
    dto.setQuestion(consultation.getQuestion());
    dto.setDate(consultation.getDate());
    dto.setTime(consultation.getTime());
    dto.setChecked(consultation.getChecked());

    return dto;
}

Fcs *Dto2ModelMapper::convertFromDto(const FcsDto &fcsDto)
{
    Fcs *fcs = new Fcs();
    fcs->setCourse(fcsDto.getCourse());
    fcs->setId(fcsDto.getId());
    fcs->setPilot(fcsDto.getPilot());
    fcs->setProgram(fcsDto.getProgram());
    fcs->setProgramName(fcsDto.getProgramName());
    fcs->setSubject(fcsDto.getSubject());
    fcs->setFormula1(fcsDto.getFormula1());
    fcs->setFormula2(fcsDto.getFormula2());
    fcs->setFormulaFinal(fcsDto.getFormulaFinal());
    fcs->setLecturer(fcsDto.getLecturer());
    fcs->setFaculty(fcsDto.getFaculty());
    fcs->setFacultyName(fcsDto.getFacultyName());

    return fcs;
}

FcsDto Dto2ModelMapper::convertFromModel(const Fcs &fcs)
{
    FcsDto fcsDto;

    fcsDto.setId(fcs.getId());
    fcsDto.setProgram(fcs.getProgram());
    fcsDto.setSubject(fcs.getSubject());
    fcsDto.setPilot(fcs.getPilot());
    fcsDto.setCourse(fcs.getCourse());
    fcsDto.setFormula1(fcs.getFormula1());
    fcsDto.setFormula2(fcs.getFormula2());
    fcsDto.setFormulaFinal(fcs.getFormulaFinal());
    fcsDto.setLecturer(fcs.getLecturer());

    return fcsDto;
}

GradeDto Dto2ModelMapper::convertFromModel(const Grade &grade)
{
    GradeDto dto;

    dto.setId(grade.getId());
    dto.setName(grade.getName());
    dto.setSurName(grade.getSurName());
    dto.setNameOfStudent(grade.getNameOfStudent());

    dto.setSubject(grade.getSubject());
    dto.setMark(grade.getMark());

    dto.setDate(grade.getDate());
    dto.setGradeTime(grade.getGradeTime());

    dto.setComment(grade.getComment());
    dto.setElementOfControl(grade.getElementOfControl());
    dto.setModule(grade.getModule());
    dto.setExaminerLogin(grade.getExaminerLogin());

    dto.setAssignmentId(grade.getAssignmentId());
    dto.setAssignmentName(grade.getAssignmentName());
    dto.setAssigned(grade.getAssigned());

    return dto;
}

Grade *Dto2ModelMapper::convertFromDto(const GradeDto &gradeDto)
{
    Grade *grade = new Grade();

    grade->setId(gradeDto.getId());
    grade->setName(gradeDto.getName());
    grade->setSurName(gradeDto.getSurName());
    grade->setNameOfStudent(gradeDto.getNameOfStudent());

    grade->setSubject(gradeDto.getSubject());
    grade->setMark(gradeDto.getMark());

    grade->setDate(gradeDto.getDate());
    grade->setGradeTime(gradeDto.getGradeTime());

    grade->setComment(gradeDto.getComment());
    grade->setElementOfControl(gradeDto.getElementOfControl());

    grade->setModule(gradeDto.getModule());
    grade->setExaminerLogin(gradeDto.getExaminerLogin());

    grade->setAssignmentId(gradeDto.getAssignmentId());
    grade->setAssignmentName(gradeDto.getAssignmentName());
    grade->setAssigned(gradeDto.getAssigned());

    return grade;
}

StudentDto Dto2ModelMapper::convertFromModel(const Student &student)
{
    StudentDto dto;
    dto.setId(student.getId());
    dto.setLogin(student.getLogin());
    dto.setGroup(student.getGroup());
    dto.setName(student.getName());
    dto.setPassword(student.getPassword());
    dto.setPilot(student.getPilot());
    dto.setCourse(student.getCourse());
    dto.setProgram(student.getProgram());
    dto.setSubgroup(student.getSubgroup());
    dto.setSurname(student.getSurname());
    dto.setAvatar(student.getAvatar());

    return dto;
}

Student *Dto2ModelMapper::convertFromDto(const StudentDto &studentDto)
{
    Student *student = new Student();

    student->setId(studentDto.getId());
    student->setCourse(studentDto.getCourse());
    student->setGroup(studentDto.getGroup());
    student->setLogin(studentDto.getLogin());
    student->setName(studentDto.getName());
    student->setPassword(studentDto.getPassword());
    student->setPilot(studentDto.getPilot());
    student->setProgram(studentDto.getProgram());
    student->setSubgroup(studentDto.getSubgroup());
    student->setSurname(studentDto.getSurname());
    student->setAvatar(studentDto.getAvatar());

    return student;
}

Teacher *Dto2ModelMapper::convertFromDto(const TeacherDto &teacherDto)
{
    Teacher *teacher = new Teacher();

    teacher->setId(teacherDto.getId());
    teacher->setGroup(teacherDto.getGroup());
    teacher->setLogin(teacherDto.getLogin());
    teacher->setPassword(teacherDto.getPassword());
    teacher->setProgram(teacherDto.getProgram());
    teacher->setSubject(teacherDto.getSubject());
    teacher->setGroup(teacherDto.getGroup());
    teacher->setSubGroup(teacherDto.getSubGroup());
    teacher->setLecturer(teacherDto.getLecturer());
    teacher->setAvatar(teacherDto.getAvatar());
    teacher->setName(teacherDto.getName());
    teacher->setSurname(teacherDto.getSurname());

    return teacher;
}

TeacherDto Dto2ModelMapper::convertFromModel(const Teacher &teacher)
{
    TeacherDto teacherDto;

    teacherDto.setId(teacher.getId());
    teacherDto.setGroup(teacher.getGroup());
    teacherDto.setLogin(teacher.getLogin());
    teacherDto.setPassword(teacher.getPassword());
    teacherDto.setProgram(teacher.getProgram());
    teacherDto.setSubject(teacher.getSubject());
    teacherDto.setGroup(teacher.getGroup());
    teacherDto.setSubGroup(teacher.getSubGroup());
    teacherDto.setLecturer(teacher.getLecturer());
    teacherDto.setAvatar(teacher.getAvatar());
    teacherDto.setName(teacher.getName());
    teacherDto.setSurname(teacher.getSurname());

    return teacherDto;
}

Work *Dto2ModelMapper::convertFromDto(const WorkDto &workDto)
{
    Work *work = new Work();

    work->setId(workDto.getId());
    work->setLogin(workDto.getLogin());
    work->setName(workDto.getName());
    work->setSurName(workDto.getSurName());
    work->setDateTime(workDto.getDateTime());
    work->setAssign(QByteArray::fromBase64(workDto.getAssign()));
    work->setIsPdf(workDto.getIsPdf());
    work->setGroup(workDto.getGroup());
    work->setSubGroup(workDto.getSubGroup());
    work->setAssignmentId(workDto.getAssignmentId());
    work->setComment(workDto.getComment());
    work->setIsLate(workDto.getIsLate());
    work->setIsChecked(workDto.getIsChecked());

    return work;
}

WorkDto Dto2ModelMapper::convertFromModel(const Work &work)
{
    WorkDto dto;

    dto.setId(work.getId());
    dto.setLogin(work.getLogin());
    dto.setName(work.getName());
    dto.setSurName(work.getSurName());
    dto.setDateTime(work.getDateTime());
    dto.setAssign(work.getAssign().toBase64());
    dto.setIsPdf(work.getIsPdf());
    dto.setGroup(work.getGroup());
    dto.setSubGroup(work.getSubGroup());
    dto.setAssignmentId(work.getAssignmentId());
    dto.setComment(work.getComment());
    dto.setIsLate(work.getIsLate());
    dto.setIsChecked(work.getIsChecked());

    return dto;
}
